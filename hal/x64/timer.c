/* -*- mode: C; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2018 Takeharu KATO                                      */
/*                                                                    */
/*  Timer handler                                                     */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

#include <hal/port.h>
#include <hal/lapic.h>

static int 
x64_timer_handler(irq_no irq __attribute__((unused)), 
    struct _exception_frame *exc __attribute__((unused)), 
    void *private __attribute__((unused))){
	

	timer_update_uptime();
	timer_update_thread_time();

	return IRQ_HANDLED;
}

void
x64_init_timer(void) {

	irq_register_handler(LAPIC_TIMER_INT_VECTOR, 
	    IRQ_ATTR_NON_NESTABLE | IRQ_ATTR_EXCLUSIVE | IRQ_ATTR_EDGE, 
	    0, NULL, x64_timer_handler);

	x64_lapic_set_timer_periodic(TIMER_MS_PER_US*CONFIG_TIMER_INTERVAL_MS);
}
