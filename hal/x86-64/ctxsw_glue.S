/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  thread context handling routines for x86-64 ABI                   */
/*                                                                    */
/**********************************************************************/
#define THR_FUNCTION_OFFSET 16  /*< スタック内での関数アドレス格納位置  */
#define THR_ARG_OFFSET      24  /*< スタック内での引数格納位置          */

.text

	.globl	__start_thread, thr_thread_start

/** x86-64用のスレッドスタート関数呼び出しルーチン
	@note x86-64は, 引数のレジスタ渡しを徹底するため, スタックからデータを取り出し,
	スタック経由で渡された関数ポインタと引数アドレスをレジスタに設定してから
	thread_start関数を呼び出す.
 */
__start_thread:
	pushq  	%rbp
	mov    	%rsp,%rbp
	movq  THR_FUNCTION_OFFSET(%rbp), %rdi
	movq  THR_ARG_OFFSET(%rbp), %rsi
	callq	thr_thread_start
	/* Never return here */
	leaveq
	retq
