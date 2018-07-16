/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Reaper thread resources                                           */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

static thread_t *reaper_thread;
static reaper_thread_info_t reaper_info;

/** Reaper thread
    @note Release resources held by terminated thread
 */
static void
reaper(void *nouse) {
	thread_t *thr;
	psw_t psw;

	
	for(;;) {
		psw_disable_and_save_interrupt(&psw);
		/*
		 * Destroy the terminated thread
		 */
		while (!thr_thread_queue_empty(&(reaper_info.reaper_queue))) {

			thr = thr_thread_queue_get_top(&(reaper_info.reaper_queue));
			thr_destroy_thread(thr);
		}
		wque_wait_on_queue(&(reaper_info.wq));  /*  Wait for next request  */
		psw_restore_interrupt(&psw);
	}
}
/** Ask the reaper thread to release thread's resources
    @param[in] thr Terminated thread
 */
void
reaper_add_exit_thread(thread_t *thr) {
	psw_t psw;

	psw_disable_and_save_interrupt(&psw);
	if (thr->status != THR_TSTATE_EXIT)
		goto out;
	thr->status = THR_TSTATE_DEAD;  /*  Set thread state as DEAD   */

	thr_add_thread_queue(&(reaper_info.reaper_queue), thr);  /*< Add it to the queue    */
	wque_wakeup(&(reaper_info.wq), WQUE_REASON_WAKEUP);  /*< Wake up the reaper thread  */
out:
	psw_restore_interrupt(&psw);
}

/** Initialize a reaper thread
 */
void
reaper_init_thread(void){
	thread_attr_t attr;

	/*
	 * Initialize some data structure for the reaper thread.
	 */
	thr_init_thread_queue(&(reaper_info.reaper_queue));
	wque_init_wait_queue(&(reaper_info.wq));
	
	/*
	 * Create the reaper thread
	 */
	memset(&attr, 0, sizeof(thread_attr_t));

	attr.prio = REAPER_THREAD_PRIO;  /*<  System thread which has a priority  */

	thr_create_thread(REAPER_THREAD_TID, &reaper_thread, &attr, reaper, NULL);
}
