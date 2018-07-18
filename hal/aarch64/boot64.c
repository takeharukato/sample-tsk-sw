/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  64bit boot code                                                   */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>
#include <kern/heap.h>

#include <hal/hal.h>
#include <hal/timer.h>
#include <hal/kmap.h>
#include <hal/boot64.h>

/** スレッドなどの初期化後のアーキ依存初期化処理
 */
void
hal_kernel_init(void) {
	thread_t *idle;
	thread_attr_t *attrp;

	idle = idle_refer_idle_thread();
	attrp = &idle->attr;

	attrp->stack_top = &bsp_stack;
	attrp->stack_size = STACK_SIZE;

	idle->tinfo = attrp->stack_top + idle->attr.stack_size - sizeof(thread_info_t);
	idle->tinfo->thr = idle;

	aarch64_init_interrupt();
	aarch64_init_timer(CONFIG_TIMER_INTERVAL_MS);
}

/** 64bit モードでのブートアップ
 */
void 
boot_main(void) {

	aarch64_uart_init();

	kprintf("OS sample for AArch64 booted.\n");

	aarch64_setup_mmu();

	kern_init();

	while(1);
}
