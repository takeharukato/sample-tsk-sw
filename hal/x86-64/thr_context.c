/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  thread context handling routines                                  */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** スレッドスタックにスレッド開始関数情報をセットする
    @param[in] thr  スレッド管理情報
    @param[in] fn   スレッドの開始関数
    @param[in] arg  スレッドの開始関数の引数
 */
void
hal_setup_thread_function(struct _thread *thr, void (*fn)(void *), void *arg) {
	addr_t *sp;
	thread_attr_t *attr = &thr->attr;

	sp = (addr_t *)thr_refer_thread_info(thr);
	--sp;  /* スレッド管理情報の一つ上から引数を積み上げる  */
	*sp-- = (addr_t)arg;
	*sp-- = (addr_t)fn;
	*sp-- = (addr_t)thr_exit_thread;  /* thr_exit threadに戻るコンテキストを念のため積んでおく  */
	*sp = (addr_t)__start_thread;     /* thread_start関数呼び出し用のグルー */
	attr->stack = sp;                 /* スタックポインタを更新する  */
}


