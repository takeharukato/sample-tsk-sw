/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  wait queue relevant routines                                      */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** スレッドを待ちキューに追加する
    @param[in] wq  待ちキュー
    @param[in] ep  待ちキューのエントリ
    @param[in] thr 待ち状態に入れるスレッド
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

/** 待ちキューからエントリを取り除く
    @param[in] wq  待ちキュー
    @param[in] ep 待ちキューのエントリ
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

/** 待ちキューを初期化する
    @param[in] wq  待ちキュー
    @param[in] ent 待ちキューのエントリ
    @param[in] thr 待ち状態に入れるスレッド
 */
void 
wque_init_wait_queue(wait_queue_t *wq) {
	psw_t psw;

	psw_disable_interrupt(&psw);
	init_list_head(&wq->head);
	psw_restore_interrupt(&psw);
}

/** 自スレッドを待ちキューを指定して事象を待ち合わせる
    @param[in] wq  待ちキュー
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

/** 待ちキューで休眠しているスレッドを起床する
    @param[in] wq  待ちキュー
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

/** ウエイトキューが空であることを確認する
    @param[in] wq ウエイトキュー
    @retval 真 ウエイトキューが空である
    @retval 偽 ウエイトキューが空でない
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
