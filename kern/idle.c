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

	for(;;) {
		
		sched_schedule();
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
