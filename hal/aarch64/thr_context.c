/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  thread context handling routines                                  */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

extern void thr_thread_start(void (*_fn)(void *), void   *_arg);

/** スレッドスタックにスレッド開始関数情報をセットする
    @param[in] thr  スレッド管理情報
    @param[in] fn   スレッドの開始関数
    @param[in] arg  スレッドの開始関数の引数
 */
void
hal_setup_thread_function(struct _thread *thr, void (*fn)(void *), void *arg) {
	uintptr_t *sp;
	thread_attr_t *attr = &thr->attr;

	sp = (uintptr_t *)thr_refer_thread_info(thr);
	--sp;  /* スレッド管理情報の一つ上から引数を積み上げる  */

	*sp-- = (uintptr_t)thr_thread_start;  /* x30 */
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;  /* x25  */
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;  /* x20  */
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;  /* x15 */
	*sp-- = (uintptr_t)0;  /* x8  */
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;  /* x5 */
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)0;
	*sp-- = (uintptr_t)arg;
	*sp-- = (uintptr_t)fn; /* x0 */
	/*
	 * AArch64では, スタックは16バイト境界であることを保証する必要があるため
	 * ダミー領域を用意する
	 * Procedure Call Standard for the ARM 64-bit Architecture(AArch64)
	 * 5.2.2.2 Stack constraints at a public interface 参照
	 */
	*sp = (uintptr_t)0xdeadbeef; /* スレッド初期化時のdummy値 */
	attr->stack = sp;                 /* スタックポインタを更新する  */
}


