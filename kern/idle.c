/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Idle thread                                                       */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

static thread_t idle_thread;        /* The thread data structure of the idle thread.     */
thread_t *current = &idle_thread;   /* Current variable indicate the idle thread on boot. */

/** Idle loop
 */
void
do_idle_loop(void) {
	thread_info_t *ti;
	psw_t psw;

	ti = hal_get_current_thread_info();

	psw_enable_interrupt();
	for(;;) {

		psw_disable_and_save_interrupt(&psw);

		sched_schedule();

		if ( !ti_check_need_dispatch(ti) )
			hal_suspend_cpu();
		psw_restore_interrupt(&psw);
	}
}

/** Return a pointer to the data structure of the idle thread
 */
thread_t *
idle_refer_idle_thread(void) {

	return &idle_thread;
}

/** Initialize idle thread
 */
void
idle_init_idle(void){
	thread_t *thr = &idle_thread;

	idle_thread.tid = 0;
	idle_thread.slice = 0;
}
