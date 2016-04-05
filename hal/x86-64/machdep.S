/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  ハード依存処理                                                    */
/*                                                                    */
/**********************************************************************/
#define ASM
#include "hal/linkage.h"
#include "hal/segments.h"

ENTRY(lgdt)
	lgdt	(%rdi)
	jmp	1f
	nop
1:
	movl	$KERN_DS, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs	
	movl	%eax, %ss

	popq    %rax
	pushq	$KERN_CS
	pushq	%rax
	lretq   /* lretqでセグメント間ジャンプして, コードセグメントを切り替える  */
END(lgdt)

IDTVEC(div)

IDTVEC(dbg)

IDTVEC(nmi)

IDTVEC(bpt)

IDTVEC(ofl)

IDTVEC(bnd)

IDTVEC(ill)

IDTVEC(dna)

IDTVEC(dblfault)

IDTVEC(fpusegm)

IDTVEC(tss)

IDTVEC(missing)

IDTVEC(stk)

IDTVEC(prot)

IDTVEC(page)

IDTVEC(fpu)

IDTVEC(align)

IDTVEC(mchk)

IDTVEC(xmm)
