/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Exception handler                                                 */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

#include <hal/exception.h>

void
hal_handle_exception(exception_frame *exc) {
	
	kprintf("An exception occur:\n");
}

void
hal_common_trap_handler(exception_frame *exc){
	thread_info_t *ti;

	ti = hal_get_current_thread_info();

	sched_delay_disptach();  /*  遅延ディスパッチ  */

	return;
}
