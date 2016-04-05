/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  アイドルスレッド                                                  */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

static thread_t idle_thread;        /* アイドルスレッドのスレッド管理情報            */
thread_t *current = &idle_thread;   /* カレントの初期値をアイドルスレッドに設定する  */

/** アイドルスレッド
    @note 終了したスレッドの回収を行う
 */
void
do_idle_loop(void) {
	thread_t *thr;
	psw_t psw;

	for(;;) {
		sched_schedule();
	}
}

/** アイドルスレッドのスレッド管理情報を返却する
 */
thread_t *
idle_refer_idle_thread(void) {

	return &idle_thread;
}

/** アイドルスレッドの初期化
 */
void
idle_init_idle(void){
	thread_t *thr = &idle_thread;

	thrmgr_reserve_threadid(0);
	idle_thread.tid = 0;
}
