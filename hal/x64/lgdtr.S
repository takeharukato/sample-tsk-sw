/* -*- mode: gas; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2018 Takeharu KATO                                      */
/*                                                                    */
/*  Load Global Descriptor Table region                               */
/*                                                                    */
/**********************************************************************/

#define ASM_FILE   1
#include <hal/segments.h>

.code64
/* void lgdtr(struct region_descriptor *rdp, uint64_t cs, uint64_t ds);  */
.global lgdtr
lgdtr:
	lgdt    (%rdi)
	movq	%ss, %rbx
	pushq	%rbx
	pushq	%rsp
	pushfq
	pushq  	%rsi
	movabsq $_reload, %r10
	pushq %r10
	/*.byte   0x48, 0xcb	*/
	iretq
_reload:
	popq    %rbx
        movq    %rdx, %es
        movq    %rdx, %ds
        movq    %rdx, %ss
	jmp    flush
        nop
flush:
	retq
	
