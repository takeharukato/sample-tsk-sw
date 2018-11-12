/* -*- mode: gas; coding:utf-8 -*- */
/************************************************************************/
/*  OS kernel sample                                                    */
/*  Copyright 2014 Takeharu KATO                                        */
/*                                                                      */
/*  Trap common                                                         */
/*                                                                      */	
/************************************************************************/

#define ASM_FILE   1
#include <hal/asm-offset.h>
#include <hal/thread_info.h>

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
enter_non_task_context:
	/*  rbx, r12-r14 is callee saved registers
	 *  in System V Application Binary Interface
	 *  AMD64 Architecture Processor Supplement
	 *  They are expected to be preserved across
	 *  function calls. So We use them here.
         */
	movq 	current(%rip), %r12             /* Current task (r12) */
	movq 	THREAD_TINFO_OFFSET(%r12), %r13 /* Current thread info (r13) */
	movq 	TI_PREEMPT_OFFSET(%r13), %r14   /* Preempt/Interrupt/Exception Count */
	andq 	$0xffff, %r14                   /* Interrupt/Exception Count > 0 */
	cmpq 	$0, %r14                        /* Is this the first interrupt? */
	jne  	call_trap_handler               /* If it isn't the first, skip changing stack */
	leaq	_intr_stack_end(%rip), %rbx   /* If it's the first, change to interrupt stack */
	movq  	%rbx, %rsp
	pushq 	%rdi  /* Save task stack on the bottom of the interrupt stack. */
call_trap_handler:
	call hal_common_trap_handler

.global ret_from_trap
ret_from_trap:
	cli
	movq 	current(%rip), %r12             /* Current task (r12) */
	movq 	THREAD_TINFO_OFFSET(%r12), %r13 /* Current thread info (r13) */
	movq 	TI_PREEMPT_OFFSET(%r13), %r14   /* Preempt/Interrupt/Exception Count */
	andq 	$0xffff, %r14                   /* Interrupt/Exception Count > 0 */
	cmpq 	$0, %r14                        /* Return to user task? */
	jne  	restore_context                 /* if we are in non-task context, just return */
	popq	%rbx                            /* Restore user task's stack */
	movq	%rbx, %rsp
	sti
	callq thr_handle_task_exception_handler
	cli
restore_context:
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
        
	add $16, %rsp /* remove trap number and errorcode */
	iretq
	
