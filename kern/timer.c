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
static call_out_que callouts={.head = __LIST_HEAD_INITIALIZER(callouts.head)};

static void
handle_callouts(void) {
	psw_t               psw;
	list_t   *lp, *next_ent;
	call_out_ent       *cur;
	list_head_t tobe_called;

	init_list_head(&tobe_called);
	psw_disable_and_save_interrupt(&psw);
	for(lp = list_ref_top(&callouts.head), next_ent = lp->next; 
	    lp != (list_t *)&callouts.head; lp = next_ent){

		next_ent = lp->next;
		cur = CONTAINER_OF(lp, call_out_ent, link);

		if ( cur->expire <= jiffies.uptime ) {

			list_del(lp);
			list_add(&tobe_called, lp);
		}
	}
	psw_restore_interrupt(&psw);

	psw_disable_and_save_interrupt(&psw);
	while( !list_is_empty(&tobe_called) ) {

		lp = list_get_top(&tobe_called);
		cur = CONTAINER_OF(lp, call_out_ent, link);

		psw_enable_interrupt();
		cur->callout(cur->private);
		psw_disable_interrupt();
	}
	psw_restore_interrupt(&psw);
}

void
timer_update_uptime(void){
	psw_t psw;

	psw_disable_and_save_interrupt(&psw);
	++jiffies.uptime;
	psw_restore_interrupt(&psw);
	handle_callouts();
}

void 
timer_update_thread_time(void){
	psw_t psw;

	psw_disable_and_save_interrupt(&psw);

	if ( current == idle_refer_idle_thread() )
		goto out;

	if ( !thr_is_round_robin_thread(current) )
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

void
callout_ent_init(call_out_ent *entp, void (*callout)(void *_private), void *private) {

	init_list_node(&entp->link);
	entp->callout = callout;
	entp->private = private;
	entp->expire = 0;

	return;
}

int
callout_add(call_out_ent *entp, uptime_cnt rel_expire){
	psw_t             psw;
	list_t *lp, *next_ent;
	uptime_cnt abs_expire;
	call_out_ent     *cur;

	kassert( entp != NULL );
	if ( ( entp->callout == NULL ) || list_is_linked(&entp->link) )
		return EINVAL;

	psw_disable_and_save_interrupt(&psw);

	abs_expire = jiffies.uptime + rel_expire;
	entp->expire = abs_expire;
	init_list_node(&entp->link);

	/*
	 * 挿入位置を探す
	 */
	for( lp = list_ref_top(&callouts.head), next_ent = lp->next;
	     lp != (list_t *)&callouts.head;
	     lp = next_ent ) {
		
		next_ent = lp->next;
		cur = CONTAINER_OF(lp, call_out_ent, link);
		if ( cur->expire >= abs_expire )
			break;
	}

	/*
	 * リストが空か要求されたタイマ発火時間が最長だった場合
	 */
	if ( lp == (list_t *)&callouts.head ) {

		list_add(&callouts.head, &entp->link);  /* リストの末尾に追加  */
		goto out;
	}

	/*
	 * 検索位置にノードを追加
	 */
	entp->link.prev = cur->link.prev;
	entp->link.next = &cur->link;
	cur->link.prev->next = &entp->link;
	cur->link.prev = &entp->link;

out:
	psw_restore_interrupt(&psw);

	return 0;
}

void
callout_init(void) {

	return;
}
