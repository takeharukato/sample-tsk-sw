/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Clean up thread resources                                         */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

static thread_t *reaper_thread;
static reaper_thread_info_t reaper_info;

/** スレッド回収スレッド
    @note 終了したスレッドの回収を行う
 */
static void
reaper(void *nouse) {
	thread_t *thr;
	psw_t psw;

	
	for(;;) {
		psw_disable_and_save_interrupt(&psw);
		/*
		 * 終了したスレッドを破棄する
		 */
		while (!thr_thread_queue_empty(&(reaper_info.reaper_queue))) {

			thr = thr_thread_queue_get_top(&(reaper_info.reaper_queue));
			thr_destroy_thread(thr);
		}
		wque_wait_on_queue(&(reaper_info.wq));  /*  次の要求がくるまで休眠する  */
		psw_restore_interrupt(&psw);
	}
}
/** 終了状態のスレッドを登録する
    @param[in] thr 終了したスレッド
 */
void
reaper_add_exit_thread(thread_t *thr) {
	psw_t psw;

	psw_disable_and_save_interrupt(&psw);
	if (thr->status != THR_TSTATE_EXIT)
		goto out;
	thr->status = THR_TSTATE_DEAD;  /* スレッド情報を破棄可能な状態に遷移する    */

	thr_add_thread_queue(&(reaper_info.reaper_queue), thr);  /*< キューに追加    */
	wque_wakeup(&(reaper_info.wq), WQUE_REASON_WAKEUP);      /*< スレッドを起床  */
out:
	psw_restore_interrupt(&psw);
}

/** 回収スレッドの初期化
 */
void
reaper_init_thread(void){
	thread_attr_t attr;

	/*
	 * 管理情報初期化
	 */
	thr_init_thread_queue(&(reaper_info.reaper_queue));
	wque_init_wait_queue(&(reaper_info.wq));
	
	/*
	 * スレッド生成
	 */
	memset(&attr, 0, sizeof(thread_attr_t));

	attr.prio = REAPER_THREAD_PRIO;  /*< 優先度付きのシステムプロセス  */

	thr_create_thread(1, &reaper_thread, &attr, reaper, NULL);

	reaper_thread->tid = REAPER_THREAD_TID;   /*< スレッドIDを設定する  */
}
