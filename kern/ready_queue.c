/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  thread ready queue                                                */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

static thread_ready_queue_t rd_queue;

/** レディーキューにスレッドを追加する
    @param[in] thr スレッド管理情報
 */
void 
rdq_add_thread(thread_t *thr){
	psw_t psw;
	thread_attr_t *attr = &thr->attr;
	thread_ready_queue_t *rdq = &rd_queue;

	psw_disable_and_save_interrupt(&psw);
	list_add(&rdq->head[attr->prio], &thr->link);
	rdq->bitmap |= (1 << attr->prio);
	psw_restore_interrupt(&psw);
}

/** レディーキューからスレッドを削除する
    @param[in] thr スレッド管理情報
 */
void 
rdq_remove_thread(thread_t *thr){
	psw_t psw;
	thread_attr_t *attr = &thr->attr;
	thread_ready_queue_t *rdq = &rd_queue;

	psw_disable_and_save_interrupt(&psw);
	thr_unlink_thread(thr);
	if (list_is_empty(&rdq->head[attr->prio]))
		rdq->bitmap &= ~(1 << attr->prio);
	psw_restore_interrupt(&psw);
}

/** ユーザスレッドのレディーキューを回転する
 */
void 
rdq_rotate_queue(void) {
	psw_t psw;
	thread_ready_queue_t *rdq = &rd_queue;

	psw_disable_and_save_interrupt(&psw);
	list_rotate(&rdq->head[RDQ_USER_RR_IDX]);
	psw_restore_interrupt(&psw);
}

/** レディーキューから動作可能なスレッドを取得する
    @param[in] rdq レディーキュー
    @retval NULL レディーキューが空だった
    @retval スレッド管理情報のアドレス レディーキュー中の実行可能スレッド
 */
thread_t *
rdq_find_runnable_thread(void){
	psw_t psw;
	thread_t *thr;
	int idx;
	thread_ready_queue_t *rdq = &rd_queue;

	psw_disable_and_save_interrupt(&psw);

	idx = find_msr_bit(rdq->bitmap);
	if (idx == 0) {
		thr = NULL;
		goto out;
	}

	thr = CONTAINER_OF(list_ref_top(&rdq->head[rdq_index2prio(idx)]), thread_t, link);
	
out:
	psw_restore_interrupt(&psw);
	
	return thr;
}

/** レディーキューを初期化する
    @param[in] que レディーキュー管理情報
 */
void
rdq_init_ready_queue(void) {
	int i;
	thread_ready_queue_t *rdq = &rd_queue;

	rdq->bitmap = 0;  /*< 動作可能なスレッドがない状態なので0に設定する  */
	for(i = 0; i < RDQ_PRIORITY_MAX; ++i){
		init_list_head(&rdq->head[i]);  /*< 各優先度のキューを初期化する  */
	}
}
