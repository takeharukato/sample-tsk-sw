/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2016 Takeharu KATO                                      */
/*                                                                    */
/*  thread context handling aarch64                                   */
/*                                                                    */
/**********************************************************************/
.text

	.globl	hal_do_context_switch

/** aarch64のスタック切り替え
    @param[in] prev 切り替えられるスレッドのスタックアドレスを格納しているポインタ変数のアドレス
    @param[in] next 切り替えるスレッドのスタックアドレスを格納しているポインタ変数のアドレス
    @note Procedure Call Standard for the ARM 64-bit Architecture
	( http://infocenter.arm.com/help/topic/com.arm.doc.ihi0055b/IHI0055B_aapcs64.pdf )
        で規定された 引数パラメタ(r0-r7, および, 間接リゾルト(r8)),
	intra-procedure-call temporary register(r16,r17), Platform Register(r18),
	callee savedレジスタ(r19-28), 
	フレームポインタ(FP=r29), リンクレジスタ(LR=r30)を保存する
	@note stp命令を使用する関係上, r15も保存している
 */
hal_do_context_switch:
	stp	x29, x30, [sp, #-16]!
	stp	x27, x28, [sp, #-16]!
	stp	x25, x26, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!		
	stp	xzr,x0, [sp, #-16]!

	# Switch stack
	mov	x19, sp
	str	x19, [x0]
	ldr	x19, [x1]
	mov	sp, x19

	# Next Thread
	ldp	x1, x0, [sp], #16  /* x1 is used for dummy  */
	ldp	x1, x2, [sp], #16
	ldp	x3, x4, [sp], #16	
	ldp	x5, x6, [sp], #16
	ldp	x7, x8, [sp], #16
	ldp	x15, x16, [sp], #16
	ldp	x17, x18, [sp], #16
	ldp	x19, x20, [sp], #16
	ldp	x21, x22, [sp], #16
	ldp	x23, x24, [sp], #16
	ldp	x25, x26, [sp], #16
	ldp	x27, x28, [sp], #16
	ldp	x29, x30, [sp], #16

	ret
