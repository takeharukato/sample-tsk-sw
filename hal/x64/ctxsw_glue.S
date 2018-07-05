/* -*- mode: asm; coding:utf-8 -*- */
/************************************************************************/
/*  OS kernel sample                                                    */
/*  Copyright 2014 Takeharu KATO                                        */
/*                                                                      */
/*  Context switch routines for x64                                     */
/*                                                                      */
/************************************************************************/

#define ASM_FILE
#include <kern/errno.h>

#define THR_FUNCTION_OFFSET  (8)  /*< スタック内での関数アドレス格納位置  */
#define THR_ARG_OFFSET      (16)  /*< スタック内での引数格納位置          */

.text

	.global	__start_thread, thr_thread_start

/** x64用のスレッドスタート関数呼び出しルーチン
	@note x64は, 引数のレジスタ渡しを徹底するため, スタックからデータを取り出し,
	スタック経由で渡された関数ポインタと引数アドレスをレジスタに設定してから
	thread_start関数を呼び出す.
 */
__start_thread:
	movq  THR_FUNCTION_OFFSET(%rsp), %rdi
	movq  THR_ARG_OFFSET(%rsp), %rsi
	movabsq $thr_thread_start, %r11
	jmp *%r11
	movq	$ENOEXEC, %rdi
	retq  /* jump to thr_exit_thread */
	
