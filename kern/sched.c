/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Thread scheduler                                                  */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

static thread_ready_queue_t rd_queue;

/** 実行可能なスレッドを選択する
    @retval NULL スケジュール可能なスレッドがいない(レディーキューが空)
    @retval 次に動作させるスレッドのスレッド管理情報
 */
static thread_t *
select_next_thread(void) {
	psw_t psw;
	thread_t *next;

	psw_disable_interrupt(&psw);
	next = rdq_find_runnable_thread(&rd_queue);

	/* 実行可能なスレッドがなければ, アイドルスレッドを選択  */	
	if (next == NULL)
		next = idle_refer_idle_thread(); 
out:
	psw_restore_interrupt(&psw);

	return next;
}

/** スレッドをスケジュール可能にする
    @param[in] thr スレッド管理情報
 */
void
sched_set_ready(thread_t *thr) {
	psw_t psw;

	psw_disable_interrupt(&psw);
	rdq_add_thread(&rd_queue, thr);  /* レディーキューに追加する  */
	psw_restore_interrupt(&psw);
}

/** レディーキューを回転する
 */
void
sched_rotate_queue(void) {
	psw_t psw;

	psw_disable_interrupt(&psw);
	rdq_rotate_queue(&rd_queue);
	psw_restore_interrupt(&psw);	
}

/** スケジューラ本体
 */
void
sched_schedule(void) {
	psw_t psw;
	thread_t *prev, *next;

	psw_disable_interrupt(&psw);

	prev = current;
	next = select_next_thread(); 
	if (prev == next) {/* ディスパッチする必要なし  */
		goto out;
	}
	current = next;
	thr_thread_switch(prev, next);

	current->status = THR_TSTATE_RUN ;
out:
	psw_restore_interrupt(&psw);	
}

/** スケジューラの初期化
 */
void
sched_init(void) {

	rdq_init_ready_queue(&rd_queue);
}
