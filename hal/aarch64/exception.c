/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Exception handler                                                 */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

#include <hal/aarch64.h>
#include <hal/exception.h>

int
hal_exception_irq_enabled(struct _exception_frame *exc){

	return !(exc->exc_spsr & AARCH64_SPSR_DAIF_I_BIT);
}

void
hal_handle_exception(exception_frame *exc) {
	
	kprintf("An exception occur:\n");
	kprintf("ESR: 0x%08p  SP: 0x%08p ELR: 0x%08p SPSR: 0x%08p\n", 
	    exc->exc_esr, exc->exc_sp, exc->exc_elr, exc->exc_spsr);
	kprintf(" x0: 0x%08p  x1: 0x%08p  x2: 0x%08p  x3: 0x%08p  x4: 0x%08p\n", 
	    exc->x0, exc->x1, exc->x2, exc->x3, exc->x4 );
	kprintf(" x5: 0x%08p  x6: 0x%08p  x7: 0x%08p  x8: 0x%08p  x9: 0x%08p\n",
	    exc->x5, exc->x6, exc->x7, exc->x8, exc->x9 );
	kprintf("x10: 0x%08p x11: 0x%08p x12: 0x%08p x13: 0x%08p x14: 0x%08p\n",
	    exc->x10, exc->x11, exc->x12, exc->x13, exc->x14 );
	kprintf("x15: 0x%08p x16: 0x%08p x17: 0x%08p x18: 0x%08p x19: 0x%08p\n",
	    exc->x15, exc->x16, exc->x17, exc->x18, exc->x19 );
	kprintf("x20: 0x%08p x21: 0x%08p x22: 0x%08p x23: 0x%08p x24: 0x%08p\n",
	    exc->x20, exc->x21, exc->x22, exc->x23, exc->x24 );
	kprintf("x25: 0x%08p x26: 0x%08p x27: 0x%08p x28: 0x%08p x29: 0x%08p\n",
	    exc->x25, exc->x26, exc->x27, exc->x28, exc->x19 );
	kprintf("x30: 0x%08p \n", exc->x30);
}

void
hal_common_trap_handler(exception_frame *exc){
	thread_info_t  *ti;

	ti = get_current_thread_info();
	if ( ( exc->exc_type & 0xf ) == AARCH64_EXC_TYPE_EXCEPTION ) {
		
		ti_update_preempt_count(ti, THR_EXCCNT_SHIFT, 1);

		if (hal_exception_irq_enabled(exc))
			psw_enable_interrupt();

		hal_handle_exception(exc);

		psw_disable_interrupt();

		ti_update_preempt_count(ti, THR_EXCCNT_SHIFT, -1);
	}

	if ( ( exc->exc_type & 0xf ) == AARCH64_EXC_TYPE_INTERRUPT ) {

		ti_update_preempt_count(ti, THR_IRQCNT_SHIFT, 1);
		irq_handle_irq(exc);
		ti_update_preempt_count(ti, THR_IRQCNT_SHIFT, -1);
	}

	return;
}

