/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Wait Queue                                                        */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

/** Add a thread into a wait queue
    @param[in] wq  wait queue
    @param[in] ep  entry of a wait queue
    @param[in] thr thread which to be wait state
 */
void 
wque_add_thread(wait_queue *wq, wait_queue_entry *ep, struct _thread  *thr){
	psw_t psw;

	psw_disable_and_save_interrupt(&psw);

	list_del(&thr->link);
	init_list_node(&ep->link);

	ep->thr = thr;
	list_add(&wq->head, &ep->link);

	psw_restore_interrupt(&psw);
}

/** Remove a entry from a wait queue
    @param[in] wq  wait queue
    @param[in] ep  wait queue entry
 */
void 
wque_remove_entry(wait_queue *wq, wait_queue_entry *ep){
	psw_t psw;

	psw_disable_and_save_interrupt(&psw);

	list_del(&ep->link);
	init_list_node(&ep->link);
	ep->thr = NULL;

	psw_restore_interrupt(&psw);
}

/** Initialize a wait queue
    @param[in] wq  wait queue
 */
void 
wque_init_wait_queue(wait_queue *wq) {
	psw_t psw;

	psw_disable_and_save_interrupt(&psw);
	init_list_head(&wq->head);
	wq->reason = WQUE_REASON_WAKEUP;
	psw_restore_interrupt(&psw);
}

/** Wait an event associated to a wait queue
    @param[in] wq  wait queue
 */
wq_reason
wque_wait_on_queue(wait_queue *wq) {
	psw_t psw;
	wait_queue_entry ent, *ep = &ent;
	wq_reason reason;

	init_list_node(&ep->link);

	psw_disable_and_save_interrupt(&psw);
	rdq_remove_thread(current);
	wque_add_thread(wq, ep, current);
	
	ep->thr->status = THR_TSTATE_WAIT;

	sched_schedule() ;

	wque_remove_entry(wq, ep);
	reason = wq->reason;
	psw_restore_interrupt(&psw);

	return reason;
}


/** Wait an event with mutex lock
    @param[in] wq  wait queue
    @param[in] mtx mutex
 */
wq_reason
wque_wait_on_event_with_mutex(wait_queue *wq, struct _mutex *mtx) {
	psw_t psw;
	wait_queue_entry ent, *ep = &ent;
	wq_reason reason;

	init_list_node(&ep->link);

	psw_disable_and_save_interrupt(&psw);

	rdq_remove_thread(current);
	wque_add_thread(wq, ep, current);
	ep->thr->status = THR_TSTATE_WAIT;

	mutex_release(mtx); 	/* We must hold the mutex during queue operations */

	sched_schedule() ;

	mutex_hold(mtx);   	/* We must hold the mutex during queue operations */

	wque_remove_entry(wq, ep);
	reason = wq->reason;
	psw_restore_interrupt(&psw);

	return reason;
}

/** Wake up all the threads in a wait queue
    @param[in] wq  wait queue
    @param[in] reason wakeup reason
 */
void 
wque_wakeup(wait_queue *wq, wq_reason reason) {
	psw_t              psw;
	wait_queue_entry   *ep;

	psw_disable_and_save_interrupt(&psw);
	while(!list_is_empty(&wq->head)) {
		ep = CONTAINER_OF(list_get_top(&wq->head), wait_queue_entry, link);

		list_del(&ep->link);
		init_list_node(&ep->link);
		ep->thr->status = THR_TSTATE_RUN;
		sched_set_ready(ep->thr); 
		ep->thr = NULL;
	}
	wq->reason = reason;
	psw_restore_interrupt(&psw);
}

/** Confirm the spcified wait queue is empty
    @param[in] wq wait queue
    @retval True The wait queue is empty
    @retval False The wait queue contains some threads.
 */
int  
is_wque_empty(wait_queue *wq) {
	int    rc;
	psw_t psw;
	
	psw_disable_and_save_interrupt(&psw);
	rc = list_is_empty(&wq->head);
	psw_restore_interrupt(&psw);
	
	return rc;
}
