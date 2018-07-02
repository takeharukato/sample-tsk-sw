/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Timer functions                                                   */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

static uptime_counter jiffies;

void
timer_update_uptime(void){
	psw_t psw;

	psw_disable_and_save_interrupt(&psw);
	++jiffies.uptime;
	psw_restore_interrupt(&psw);
}

void 
timer_update_thread_time(void){
	psw_t psw;

	psw_disable_and_save_interrupt(&psw);

	if ( current == idle_refer_idle_thread())
		goto out;

	if ( current->slice > 0 )
		--current->slice;

	if ( current->slice == 0 ) {

		current->slice = CONFIG_TIMER_TIME_SLICE;  /* Reload the time slice */
		/*
		 * Dispatch to the next thread
		 */
		sched_rotate_queue();
		sched_schedule();
	}
out:
	psw_restore_interrupt(&psw);
}
