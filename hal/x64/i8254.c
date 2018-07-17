/* -*- mode: C; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2018 Takeharu KATO                                      */
/*                                                                    */
/*  Intel 8254 Programmable Interval Timer(PIT) relevant routines     */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

#include <hal/port.h>
#include <hal/i8254.h>

#define X64_TIMER_IRQ (0)

static int 
x64_timer_handler(irq_no irq __attribute__((unused)), 
    struct _exception_frame *exc __attribute__((unused)), 
    void *private __attribute__((unused))){

	timer_update_uptime();
	timer_update_thread_time();
	
	return IRQ_HANDLED;
}

void
x64_timer_init(void) {
	uint32_t  divisor;

	divisor = I8254_INPFREQ / (1000 / CONFIG_TIMER_INTERVAL_MS);
	 
	/* Square Wave(方形波ジェネレータ)モード, バイナリカウンタ, 16bitカウンタに設定  */
	out_port_byte(I8254_PORT_MODECNTL, I8254_CMD_INTERVAL_TIMER);
	/* 下位バイト, 上位バイトの順に,  チャネル0に周波数を設定  */
	out_port_byte(I8254_PORT_CHANNEL0, (uint8_t)( divisor & ~( (uint8_t)(0) ) ) );
	out_port_byte(I8254_PORT_CHANNEL0, (uint8_t)( (divisor >> 8) & ~( (uint8_t)(0) ) ) );

	irq_register_handler(X64_TIMER_IRQ, 
	    IRQ_ATTR_NON_NESTABLE | IRQ_ATTR_EXCLUSIVE | IRQ_ATTR_EDGE, 
	    0, NULL, x64_timer_handler);
}
