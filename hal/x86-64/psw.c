/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Processor Status Word                                             */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** CPUレベルで割込みを禁止する
    @param[in] pswp 割り込み禁止前のプロセサステータスワード返却域
 */
void
psw_disable_interrupt(psw_t *pswp) {
	psw_t psw;

	__save_psw(psw);
	*pswp = psw;
}

/** CPUレベルで割込み状態を復元する
    @param[in] pswp プロセサステータスワード返却域
 */
void
psw_restore_interrupt(psw_t *pswp) {
	psw_t psw;

	psw = *pswp;
	__restore_psw(psw);
}
