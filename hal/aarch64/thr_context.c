/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  thread context handling routines                                  */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

extern void thr_thread_start(void (*_fn)(void *), void   *_arg);

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

	*sp-- = (addr_t)thr_thread_start;  /* x30 */
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;  /* x25  */
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;  /* x20  */
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;  /* x15 */
	*sp-- = (addr_t)0;  /* x8  */
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;  /* x5 */
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)arg;
	*sp-- = (addr_t)fn; /* x0 */
	*sp = 0xdeadbeef; /* dummy */
	attr->stack = sp;                 /* スタックポインタを更新する  */
}


