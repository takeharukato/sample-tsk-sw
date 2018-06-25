/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  wait queue                                                        */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** Add a thread into a wait queue
    @param[in] wq  wait queue
    @param[in] ep  entry of a wait queue
    @param[in] thr thread which to be wait state
 */
void 
wque_add_thread(wait_queue_t *wq, wait_queue_entry_t *ep, struct _thread  *thr){
	psw_t psw;

	psw_disable_interrupt(&psw);

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
wque_remove_entry(wait_queue_t *wq, wait_queue_entry_t *ep){
	psw_t psw;

	psw_disable_interrupt(&psw);

	list_del(&ep->link);
	init_list_node(&ep->link);
	ep->thr = NULL;

	psw_restore_interrupt(&psw);
}

/** Initialize a wait queue
    @param[in] wq  wait queue
 */
void 
wque_init_wait_queue(wait_queue_t *wq) {
	psw_t psw;

	psw_disable_interrupt(&psw);
	init_list_head(&wq->head);
	psw_restore_interrupt(&psw);
}

/** Wait an event associated to a wait queue
    @param[in] wq  wait queue
 */
void 
wque_wait_on_queue(wait_queue_t *wq) {
	psw_t psw;
	wait_queue_entry_t ent, *ep = &ent;

	init_list_node(&ep->link);

	psw_disable_interrupt(&psw);
	if (THR_THREAD_ON_RDQ(current))
		rdq_remove_thread(current->rdq, current);
	wque_add_thread(wq, ep, current);
	
	ep->thr->status = THR_TSTATE_WAIT;

	sched_schedule() ;

	wque_remove_entry(wq, ep);

	psw_restore_interrupt(&psw);
}

/** Wake up all the threads in a wait queue
    @param[in] wq  wait queue
 */
void 
wque_wakeup(wait_queue_t *wq) {
	psw_t                psw;
	wait_queue_entry_t   *ep;

	psw_disable_interrupt(&psw);
	while(!list_is_empty(&wq->head)) {
		ep = CONTAINER_OF(list_get_top(&wq->head), wait_queue_entry_t, link);

		list_del(&ep->link);
		init_list_node(&ep->link);
		ep->thr->status = THR_TSTATE_RUN;
		sched_set_ready(ep->thr); 
		ep->thr = NULL;
	}
	
	psw_restore_interrupt(&psw);
}

/** Confirm the spcified wait queue is empty
    @param[in] wq wait queue
    @retval True The wait queue is empty
    @retval False The wait queue contains some threads.
 */
int  
is_wque_empty(wait_queue_t *wq) {
	int    rc;
	psw_t psw;
	
	psw_disable_interrupt(&psw);
	rc = list_is_empty(&wq->head);
	psw_restore_interrupt(&psw);
	
	return rc;
}
