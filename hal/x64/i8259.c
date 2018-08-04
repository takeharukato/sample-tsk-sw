/* -*- mode: C; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2018 Takeharu KATO                                      */
/*                                                                    */
/*  Intel 8259 Programmable Interrupt Controller(PIC) routines        */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

#include <hal/port.h>
#include <hal/i8259.h>
#include <hal/exception.h>

#define I8259_MAX_IRQ_NR (16)  /* Intel i8259の割込み数  */

static void i8259_enable_irq(struct _irq_ctrlr *_ctrlr, irq_no _no);
static void i8259_disable_irq(struct _irq_ctrlr *_ctrlr, irq_no _no);
static void i8259_eoi_irq(struct _irq_ctrlr *_ctrlr, irq_no _no);
static int i8259_config_irq(struct _irq_ctrlr *_ctrlr, 
    irq_no _irq, irq_attr _attr, irq_prio _prio);
static int init_i8259_pic(struct _irq_ctrlr *_ctrlr);
static void finalize_i8259_pic(struct _irq_ctrlr *_ctrlr);
static int i8259_find_pending_irq(struct _exception_frame *_exc, irq_no *_irqp);

static irq_ctrlr i8259_ctrlr = {
	.config_irq = i8259_config_irq,
	.enable_irq = i8259_enable_irq,
	.disable_irq = i8259_disable_irq,
	.eoi = i8259_eoi_irq,
	.initialize = init_i8259_pic,
	.finalize = finalize_i8259_pic,
	.private = NULL
};

static int 
i8259_config_irq(struct _irq_ctrlr *ctrlr __attribute__((unused)), 
    irq_no irq, irq_attr attr, irq_prio prio){

	return 0;
}
/** Intel i8259で指定したIRQのi8259の割込みマスクを解除する
    @param[in] irq 割込み番号
    @retval 0   正常終了
 */
static void
i8259_enable_irq(struct _irq_ctrlr *ctrlr __attribute__((unused)), irq_no no) {
	uint8_t mask;
	uint8_t irq;

	kassert( no < I8259_MAX_IRQ_NR );
	irq = no & ~((uint8_t)0);

	if (irq < 8)  { /*  マスター  */

		/* 現在のマスクから指定されたIRQのマスクを取り除いたマスク値を算出  */
		mask = in_port_byte(I8259_PIC1_BASE_ADDR2) & ~( 1 << irq );
		/* 算出したマスクを設定 */
		out_port_byte(I8259_PIC1_BASE_ADDR2, mask);
	} else {  /*  スレーブ  */

		/* 現在のマスクから指定されたIRQのマスクを取り除いたマスク値を算出  */
		mask = in_port_byte(I8259_PIC2_BASE_ADDR2) & ~( 1 << (irq - 8) );
		/* 算出したマスクを設定 */
		out_port_byte (I8259_PIC2_BASE_ADDR2, mask);
	}

	return ;
}

/** Intel i8259で指定したIRQのi8259の割込みをマスクする
    @param[in] irq 割込み番号
    @retval 0   正常終了
 */
static void
i8259_disable_irq(struct _irq_ctrlr *ctrlr __attribute__((unused)), irq_no no) {
	uint8_t mask;
	uint8_t irq;

	irq = no & ~((uint8_t)0);

	kassert( irq < I8259_MAX_IRQ_NR );

	if (irq < 8) { /*  マスター  */

		/* 現在のマスク値と指定されたマスクのORを取り更新後のマスクを算出  */
		mask = in_port_byte(I8259_PIC1_BASE_ADDR2) | ( 1 << irq ) ;
		/* 算出したマスクを設定 */
		out_port_byte(I8259_PIC1_BASE_ADDR2, mask);
	} else {  /*  スレーブ  */

		/* 現在のマスク値と指定されたマスクのORを取り更新後のマスクを算出  */
		mask = in_port_byte(I8259_PIC2_BASE_ADDR2) | ( 1 << (irq - 8)) ;
		/* 算出したマスクを設定 */
		out_port_byte(I8259_PIC2_BASE_ADDR2, mask);
	}

	return ;
}

/** Intel i8259で指定したIRQのi8259の割込みに対するEnd of Interruptを発行する
    @param[in] irq 割込み番号
    @retval 0   正常終了
    @note ISRクリア中に割り込まないよう対象のIRQをマスクしてから呼び出すこと
 */
static void
i8259_eoi_irq(struct _irq_ctrlr *ctrlr __attribute__((unused)), irq_no no) {
	uint8_t irq;
	uint8_t mask;

	irq = no & ~((uint8_t)0);

	kassert( irq < I8259_MAX_IRQ_NR );

	/*
	 * ISRクリア中に割り込まないよう対象のIRQをマスク済みであることを確認する
	 */
	if (irq < 8) {  /*  マスター  */
	
		mask = in_port_byte(I8259_PIC1_BASE_ADDR2);
		kassert( mask & ( 1 << irq ) );
	} else {
		
		mask = in_port_byte(I8259_PIC2_BASE_ADDR2);
		kassert( mask & ( 1 << ( irq - 8 ) ) );
	}


	/*
	 * ISRをクリアする
	 */
	if ( irq < 8 )   /* マスタ側にEOIを投げる  */
		out_port_byte(I8259_PIC1_OCW2_ADDR, pic_mk_spec_eoi(irq));
	else {
		/*
		 * カスケード接続されたi8259の場合, スレーブにEOIを投げた後で,
		 * マスターのカスケード接続IRQに対してEOIを投げる。
		 * スレーブの割込みは, マスターコントローラを介して, 
		 * CPUへ通知されるためマスター側で割込みをキューイングできる
		 * ようにマスタのカスケード接続IRQのキューをクリアするためである。
		 */

		/*  スレーブ側のコントローラにEOIを投げる  */
		out_port_byte(I8259_PIC2_OCW2_ADDR, pic_mk_spec_eoi(irq)); 
		/*  マスタ側のコントローラにEOIを投げる  */
		out_port_byte(I8259_PIC1_OCW2_ADDR, pic_mk_spec_eoi(I8259_PIC1_ICW3_CODE));
	}

	return ;
	
}

/** 割込みマスクを設定する
    @param[in] msk 設定するマスク
 */
void
i8259_update_irq_mask(intr_mask_state msk) {
	uint16_t i8259_mask;

	i8259_mask = msk & ~( (uint16_t)0 );
	i8259_mask &= ~( 1 << I8259_PIC1_ICW3_CODE );  /*  カスケード接続は空ける  */

	out_port_byte(I8259_PIC1_BASE_ADDR2, ( i8259_mask & ~((uint8_t)0) ) );
	out_port_byte(I8259_PIC2_BASE_ADDR2, ( i8259_mask >> 8 ) );
}

/** i8259割込みコントローラの初期化 
    IRQ0以降を割り込みベクタ0x20以降に割込むようにコントローラを設定し,
    CPU例外との衝突が起こらないようにする。
    また, 初期状態として, PIC1とPIC2との接続箇所を除き割込みをマスク
    しておく。
 */
static int
init_i8259_pic(struct _irq_ctrlr *ctrlr){
	int i;

	/* PIC 1*/
	out_port_byte(I8259_PIC1_BASE_ADDR1, I8259_PIC1_ICW1_CODE);
	out_port_byte(I8259_PIC1_BASE_ADDR2, I8259_PIC1_ICW2_CODE);
	out_port_byte(I8259_PIC1_BASE_ADDR2, I8259_PIC1_ICW3_CODE);
	out_port_byte(I8259_PIC1_BASE_ADDR2, I8259_PIC1_ICW4_CODE);

	/* PIC2  */
	out_port_byte(I8259_PIC2_BASE_ADDR1, I8259_PIC2_ICW1_CODE);
	out_port_byte(I8259_PIC2_BASE_ADDR2, I8259_PIC2_ICW2_CODE);
	out_port_byte(I8259_PIC2_BASE_ADDR2, I8259_PIC2_ICW3_CODE);
	out_port_byte(I8259_PIC2_BASE_ADDR2, I8259_PIC2_ICW4_CODE);

	/*
	 * 初期状態として割込みを全マスクする
	 */
	for(i = 0; I8259_MAX_IRQ_NR > i; ++i ) {

		if ( i == I8259_PIC1_ICW3_CODE )
			continue;
		i8259_disable_irq(&i8259_ctrlr, (uint8_t)(i));
	}

	i8259_enable_irq(&i8259_ctrlr, I8259_PIC1_ICW3_CODE);  /* IRQ2をカスケード接続用に空ける  */

	ctrlr->private = NULL;

	return 0;
}

static void
finalize_i8259_pic(struct _irq_ctrlr *_ctrlr __attribute__((unused))){
	int i;

	/*
	 * 割込みを全てマスクする
	 */
	for(i = 0; I8259_MAX_IRQ_NR > i; ++i ) {

		if ( i == I8259_PIC1_ICW3_CODE )
			continue;
		i8259_disable_irq(&i8259_ctrlr, (uint8_t)(i));
	}

	i8259_enable_irq(&i8259_ctrlr, I8259_PIC1_ICW3_CODE);  /* IRQ2をカスケード接続用に空ける  */
}

static int 
i8259_find_pending_irq(struct _exception_frame *exc, irq_no *irqp){
	irq_no irq;

	if ( exc->trapno < I8259_PIC1_VBASE_ADDR )
		return IRQ_NOT_FOUND;

	irq = exc->trapno - I8259_PIC1_VBASE_ADDR;

	if ( irq >= I8259_MAX_IRQ_NR )
		return IRQ_NOT_FOUND;

	*irqp = irq;

	return IRQ_FOUND;
}

void
x64_init_pic(void){
	int i;

	init_i8259_pic(&i8259_ctrlr);

	irq_register_pending_irq_finder(i8259_find_pending_irq);

	for(i = 0; I8259_MAX_IRQ_NR > i; ++i) 
		irq_register_ctrlr(i, &i8259_ctrlr);

}
