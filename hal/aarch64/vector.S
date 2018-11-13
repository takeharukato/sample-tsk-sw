/* -*- mode: asm; coding:utf-8 -*- */
/************************************************************************/
/*  OS kernel sample                                                    */
/*  Copyright 2014 Takeharu KATO                                        */
/*                                                                      */
/*  Exception vector                                                    */
/*                                                                      */
/************************************************************************/
#define ASM_FILE   1

#include <hal/aarch64.h>
#include <hal/exception.h>
#include <hal/asm-macros.h>
#include <hal/asm-offset.h>

#define __CONTEXT_REG	x20  /* Exception pointer ( Note: x20 is a callee saved register). */
	
.section ".text"

.extern hal_common_trap_handler
.extern thr_handle_task_exception_handler
.extern _intr_stack_end

.macro build_trapframe exc_type
	/*
	 * store generic registers from (x29,x30) pair to (x1,x2) pair.
	 */
	stp	x29, x30, [sp, #-16]!
	stp	x27, x28, [sp, #-16]!
	stp	x25, x26, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!
	/*
	 * Store (spsr, x0)
	 */
	mrs	x21, spsr_el1
	stp	x21, x0, [sp, #-16]!
	/*
	 * Allocate a room for sp_el0 and store elr
	 */
	mrs	x21, elr_el1
	stp	xzr, x21, [sp, #-16]!
	/*
	 * store exception type and esr
	 */
	mov	x21, #(\exc_type)
	mrs	x22, esr_el1
	stp	x21, x22, [sp, #-16]!
	mov	__CONTEXT_REG, sp
.endm

.macro store_traped_sp
	mrs	x21, sp_el0
	str	x21, [sp, #EXC_EXC_SP_OFFSET]
.endm

.macro handle_task_exceptions
	mov	x0, __CONTEXT_REG
	bl	thr_handle_task_exception_handler
.endm

.macro enter_non_task_context
	ldr	x21, =current                    /* Current task (x21) */
	ldr	x21, [x21]	
	ldr	x22, [x21, #THREAD_TINFO_OFFSET] /* Current thread info (x22) */
	ldr	x23, [x22, #TI_PREEMPT_OFFSET]   /* Preempt/Interrupt/Exception Count */
	and	x23, x23, #0xffff                /* Interrupt/Exception Count > 0 */
	cmp	x23, #0
	b.ne	1f
	/* This is the first interrupt, we need to change our stack to the interrupt stack */
	ldr	x23, =_intr_stack_end
	mov	sp, x23
	stp	xzr, __CONTEXT_REG, [sp, #-16]!	 /* Store task context stack pointer */
1:	
.endm

.macro leave_non_task_context
	ldr	x21, =current                    /* Current task (x21) */
	ldr	x21, [x21]
	ldr	x22, [x21, #THREAD_TINFO_OFFSET] /* Current thread info (x22) */
	ldr	x23, [x22, #TI_PREEMPT_OFFSET]   /* Preempt/Interrupt/Exception Count */
	and	x23, x23, #0xffff                /* Interrupt/Exception Count > 0 */
	cmp	x23, #0
	b.ne	1f
	/* Return to user task */
	ldp	x23, __CONTEXT_REG, [sp], #16	 /* Restore task context stack pointer */
	mov	sp, __CONTEXT_REG
	handle_task_exceptions
1:
.endm

.macro call_common_trap_handler
	mov	x0, __CONTEXT_REG
	bl 	hal_common_trap_handler
.endm

.macro store_nested_sp
	mov	x21, sp 	
	add	x21, x21, #EXC_FRAME_SIZE
	str	x21, [sp, #EXC_EXC_SP_OFFSET]
.endm

.macro restore_traped_sp
	ldr	x21, [sp, #EXC_EXC_SP_OFFSET]
	msr	sp_el0, x21
.endm

.macro restore_trapframe

	/*
	 * Drop exception type, esr
	 */
	add	sp, sp, #16
	/*
	 * Drop exception stack pointer and restore elr_el1
	 */
	ldp	x21, x22, [sp], #16	
	msr	elr_el1, x22

	/*
	 * Retore spsr and x0
	 */
	ldp	x21, x0, [sp], #16
	msr	spsr_el1, x21

	/*
	 * Restore generic registers from (x29,x30) pair to (x1,x2) pair.
	 */
	ldp	x1, x2, [sp], #16
	ldp	x3, x4, [sp], #16
	ldp	x5, x6, [sp], #16
	ldp	x7, x8, [sp], #16
	ldp	x9, x10, [sp], #16
	ldp	x11, x12, [sp], #16
	ldp	x13, x14, [sp], #16
	ldp	x15, x16, [sp], #16
	ldp	x17, x18, [sp], #16
	ldp	x19, x20, [sp], #16
	ldp	x21, x22, [sp], #16
	ldp	x23, x24, [sp], #16
	ldp	x25, x26, [sp], #16
	ldp	x27, x28, [sp], #16
	ldp	x29, x30, [sp], #16

	eret
.endm
	
/*
 * Exception vectors.
 */
	vector_base_align
	.globl	vectors
vectors:
	/*
	 * Current EL with SP0(e.g.,Trapped from User Task in EL1)
	 */
        vector_entry_align
	b	_el1_user_sync	/* Synchronous */
        vector_entry_align
	b	_el1_user_irq	/* IRQ/vIRQ */
        vector_entry_align
	b	_el1_user_fiq	/* FIQ/vFIQ */
        vector_entry_align
	b	_el1_user_error	/* SError/vSError */

	/*
	 * Current EL with SPx(e.g.,Interrupted from Kernel in EL1)
	 */
        vector_entry_align
	b	_el1_sync	/* Synchronous */
        vector_entry_align
	b	_el1_irq	/* IRQ/vIRQ */
        vector_entry_align
	b	_el1_fiq	/* FIQ/vFIQ */
        vector_entry_align
	b	_el1_error	/* SError/vSError */

	/*
	 * Lower EL (e.g., EL0/SP0) using AArch64
	 */
        vector_entry_align
	b	_el0_sync64	/* Synchronous from AArch64 */
        vector_entry_align
	b	_el0_irq64	/* IRQ EL0 from AArch64 */
        vector_entry_align
	b	_el0_fiq64	/* FIQ EL0 from AArch64 */
        vector_entry_align
	b	_el0_error64	/* Error EL0 from AArch64 */

	/*
	 * Lower EL (e.g., EL0/SP0) using AArch32
	 */
        vector_entry_align
	b	_el0_sync32	/* Synchronous from AArch32 */
        vector_entry_align
	b	_el0_irq32	/* IRQ EL0 from AArch32 */
        vector_entry_align
	b	_el0_fiq32	/* FIQ EL0 from AArch32 */
        vector_entry_align
	b	_el0_error32	/* Error EL0 from AArch32 */

	text_align
_el1_user_sync:	/* Synchronous */
	build_trapframe AARCH64_EXC_SYNC_SP0
	store_traped_sp
	enter_non_task_context
	call_common_trap_handler
	leave_non_task_context
	restore_traped_sp
	restore_trapframe

	text_align
_el1_user_irq:	/* IRQ/vIRQ */
	build_trapframe AARCH64_EXC_IRQ_SP0
	store_traped_sp
	enter_non_task_context
	call_common_trap_handler
	leave_non_task_context
	restore_traped_sp
	restore_trapframe

	text_align
_el1_user_fiq:	/* FIQ/vFIQ */
	build_trapframe AARCH64_EXC_FIQ_SP0
	store_traped_sp
	enter_non_task_context	
	call_common_trap_handler
	leave_non_task_context
	restore_traped_sp
	restore_trapframe

	text_align
_el1_user_error:	/* SError/vSError */
	build_trapframe AARCH64_EXC_SERR_SP0
	store_traped_sp
	enter_non_task_context	
	call_common_trap_handler
	leave_non_task_context
	restore_traped_sp
	restore_trapframe

	text_align
_el1_sync:	/* Synchronous */
	build_trapframe AARCH64_EXC_SYNC_SPX
	store_nested_sp
	enter_non_task_context	
	call_common_trap_handler
	leave_non_task_context	
	restore_trapframe

	text_align
_el1_irq:	/* IRQ/vIRQ */
	build_trapframe AARCH64_EXC_IRQ_SPX	
	store_nested_sp
	enter_non_task_context	
	call_common_trap_handler
	leave_non_task_context
	restore_trapframe

	text_align
_el1_fiq:	/* FIQ/vFIQ */
	build_trapframe AARCH64_EXC_FIQ_SPX
	store_nested_sp
	enter_non_task_context	
	call_common_trap_handler
	leave_non_task_context
	restore_trapframe

	text_align
_el1_error:	/* SError/vSError */
	build_trapframe AARCH64_EXC_SERR_SPX	
	store_nested_sp
	enter_non_task_context	
	call_common_trap_handler
	leave_non_task_context
	restore_trapframe
	
	text_align
_el0_sync64:	/* Synchronous from AArch64 */
	build_trapframe AARCH64_EXC_SYNC_AARCH64
	store_traped_sp
	enter_non_task_context	
	call_common_trap_handler
	leave_non_task_context
	restore_traped_sp
	restore_trapframe
	
	text_align
_el0_irq64:	/* IRQ EL0 from AArch64 */
	build_trapframe AARCH64_EXC_IRQ_AARCH64	
	store_traped_sp
	enter_non_task_context	
	call_common_trap_handler
	leave_non_task_context	
	restore_traped_sp
	restore_trapframe
	
	text_align
_el0_fiq64:	/* FIQ EL0 from AArch64 */
	build_trapframe AARCH64_EXC_FIQ_AARCH64
	store_traped_sp
	enter_non_task_context	
	call_common_trap_handler
	leave_non_task_context
	restore_traped_sp
	restore_trapframe
	
	text_align
_el0_error64:	/* Error EL0 from AArch64 */
	build_trapframe AARCH64_EXC_SERR_AARCH64	
	store_traped_sp
	enter_non_task_context	
	call_common_trap_handler
	leave_non_task_context	
	restore_traped_sp
	restore_trapframe


	text_align
_el0_sync32:	/* Synchronous from AArch32 */
	build_trapframe AARCH64_EXC_SYNC_AARCH32	
	store_traped_sp
	enter_non_task_context	
	call_common_trap_handler
	leave_non_task_context	
	restore_traped_sp
	restore_trapframe
	
	text_align
_el0_irq32:	/* IRQ EL0 from AArch32 */
	build_trapframe AARCH64_EXC_IRQ_AARCH32
	store_traped_sp
	enter_non_task_context	
	call_common_trap_handler
	leave_non_task_context
	restore_traped_sp
	restore_trapframe
	
	text_align
_el0_fiq32:	/* FIQ EL0 from AArch32 */
	build_trapframe AARCH64_EXC_FIQ_AARCH32
	store_traped_sp
	enter_non_task_context	
	call_common_trap_handler
	leave_non_task_context	
	restore_traped_sp
	restore_trapframe
	
	text_align
_el0_error32:	/* Error EL0 from AArch32 */
	build_trapframe AARCH64_EXC_SERR_AARCH32
	store_traped_sp
	enter_non_task_context	
	call_common_trap_handler
	leave_non_task_context	
	restore_traped_sp
	restore_trapframe

	text_align
 	.globl	aarch64_setup_vector
aarch64_setup_vector:	
	ldr x0, =vectors
	msr vbar_el1, x0
	ret
	
