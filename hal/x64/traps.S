/* -*- mode: gas; coding:utf-8 -*- */
/************************************************************************/
/*  OS kernel sample                                                    */
/*  Copyright 2014 Takeharu KATO                                        */
/*                                                                      */
/*  Trap common                                                         */
/*                                                                      */	
/************************************************************************/

#define ASM_FILE   1

.code64
.global build_trap_context
build_trap_context:
	cli
	cld
	pushq %r15
	pushq %r14
	pushq %r13
	pushq %r12
	pushq %r11
	pushq %r10
	pushq %r9
	pushq %r8
	pushq %rdi
	pushq %rsi
	pushq %rbp
	pushq %rdx
	pushq %rcx
	pushq %rbx
	pushq %rax

	movq  %rsp, %rdi  
	call hal_common_trap_handler

.global ret_from_trap
	cli
	/*  例外から復帰する  */
ret_from_trap:
	popq %rax
	popq %rbx
	popq %rcx
	popq %rdx
	popq %rbp
	popq %rsi
	popq %rdi
	popq %r8
	popq %r9
	popq %r10
	popq %r11
	popq %r12
	popq %r13
	popq %r14
	popq %r15
        # remove trap number and errorcode
	add $16, %rsp
	iretq
	
