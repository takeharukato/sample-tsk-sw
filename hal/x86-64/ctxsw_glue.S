/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  thread context handling routines for x86-64 ABI                   */
/*                                                                    */
/**********************************************************************/
#define THR_FUNCTION_OFFSET  8  /*< スタック内での関数アドレス格納位置  */
#define THR_ARG_OFFSET      16  /*< スタック内での引数格納位置          */
#define	ENOEXEC		 8	/* Exec format error */
.text

	.globl	__start_thread, thr_thread_start

/** x86-64用のスレッドスタート関数呼び出しルーチン
	@note x86-64は, 引数のレジスタ渡しを徹底するため, スタックからデータを取り出し,
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
	
