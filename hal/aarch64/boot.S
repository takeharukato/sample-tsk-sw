/* -*- mode: asm; coding:utf-8 -*- */
/************************************************************************/
/*  OS kernel sample                                                    */
/*  Copyright 2014 Takeharu KATO                                        */
/*                                                                      */
/*  start up routines for aarch64                                       */
/************************************************************************/

#define ASM_FILE   1

#include <kern/param.h>

#include <hal/thread_info.h>
#include <hal/asm-offset.h>	
#include <hal/aarch64.h>

.extern bsp_stack
.extern pre_ttbr0_l0tbl
.extern pre_ttbr0_l1tbl
.extern pre_ttbr1_l0tbl
.extern pre_ttbr1_l1tbl

.global _start
start:	
_start:	
	msr   daifset, #(AARCH64_DAIF_FIQ | AARCH64_DAIF_IRQ) /* Disable FIQ and IRQ */

	mrs   x0, CurrentEL
	and   x0, x0, #AARCH64_CUREL_MASK
	cmp   x0, #AARCH64_EL_EL3
	b.eq  drop_el2
	cmp   x0, #AARCH64_EL_EL2
	b.eq  drop_el1
	b     start_el1
drop_el2:
	/*
	 * Drop into EL2 from EL3
	 */
	mov   x0, #(AARCH64_SCR_EL3_NS | AARCH64_SCR_EL3_RES | AARCH64_SCR_EL3_SMA | AARCH64_SCR_EL3_RW)
	msr   scr_el3, x0
	adr   x0, start_el2
	msr   elr_el3, x0
	mov   x0, #(AARCH64_SPSR_FROM_AARCH64 | AARCH64_SPSR_MODE_EL2 | AARCH64_SPSR_SP_SEL_N)
	msr   spsr_el3, x0
	eret
start_el2:
drop_el1:
	/*
	 * Drop into EL1 from EL2
	 */
	mov   x0, #( AARCH64_HCR_EL2_RW )
	orr   x0, x0,#( AARCH64_HCR_EL2_SWIO ) 
	msr   hcr_el2, x0
	adr   x0, start_el1
	msr   elr_el2, x0
	mov   x0, #( AARCH64_SPSR_DAIF_D_BIT  | AARCH64_SPSR_DAIF_A_BIT | AARCH64_SPSR_DAIF_I_BIT | AARCH64_SPSR_DAIF_F_BIT | AARCH64_SPSR_FROM_AARCH64 | AARCH64_SPSR_MODE_EL1 | AARCH64_SPSR_SP_SEL_N)
	msr   spsr_el2, x0
	eret
start_el1:	
	/*
	 * See. Example 5-10 Clean and invalidate L1 data cache in
	 * Bare-metal Boot Code for ARMv8-A Processors
	 */

	/*
	 * Disable L1 Cache
	 */
	mrs	x0, sctlr_el1
	bic	x0, x0, #(AARCH64_SCTLR_C)
	msr	sctlr_el1, x0

	/*
	 *  Invalidate Data cache
	 */
	mov	x0, #0x0
	msr	csselr_el1, x0	/* Cache level 0 to L1 Dcache */

	/*
	 * Load Cache Line Size to x1
	 * Note:  Example 11-3 Cleaning to Point of Coherency in
	 * ARM Cortex-A Series Programmer’s Guide for ARMv8-A
	 * says that CCSIDR returns /log2(linelen)-2.
	 * I'm not sure which is correct.
	 * the following codes relys on the fact that
	 * ARM Cortex-A53 MPCore Processor Technical Reference Manual is correct.
	 */
	mrs	x4, ccsidr_el1
	and	x1, x4, #0x7  //log2(linelen)-2
	add	x1, x1, #0x2  //log2(linelen)
	/*
	 * Load Cache Set Number -1 to x2
	 */
	mov	x3, #0x7fff
	and	x2, x3, x4, lsr #( AARCH64_CCSIDR_NUMSETS_SHIFT )

	/*
	 * Load Cache Associativity Number - 1 to x3
	 */
	mov	x3, #0x3ff
	and	x3, x3, x4, lsr #( AARCH64_CCSIDR_ASCTY_SHIFT )

	/*
	 * way position in the CISW instruction to x4
	 */
	clz	w4, w3

	mov	x5, #(0)  /* x5 = way counter way_loop */
way_loop:
	mov	x6, #(0)  /* x6 = set counter set_loop */
set_loop:
	lsl	x7, x5, x4
	orr	x7, x0, x7	/*  Set way. */
	lsl	x8, x6, x1
	orr	x7, x7, x8	/*  Set set. */
	dc	cisw, x7	/*  Clean and Invalidate cache line. */
	add	x6, x6, #1 	/*  Increment set counter. */
	cmp	x6, x2 		/*  Last set reached yet?  */
	ble	set_loop 	/*  If not, iterate set_loop */

	add	x5, x5, #1	/*  else, next way.  */
	cmp	x5, x3		/*  last way reached yet?  */
	ble	way_loop 	/*  If not, iterate way_loop.  */
	
	/*
	 * Suspend cpus except cpuid == 0.
	 */
suspend_aps:
	mrs     x1, mpidr_el1  /*  Read cpuid                                */
	and     x1, x1, #3     /*  Check whether CPUID is zero or not       */
	cbz     x1, clear_bss  /*  If CPUID is zero, continue to initialize */
ap_wait_loop:
	wfe                    /*  Enter WFE(Wait For Event) loop if CPUID is NOT zero. */
	b       ap_wait_loop

clear_bss:
	/* clear bss */
	ldr     x1, =__bss_start
	ldr     x2, =__bss_end
	sub	x2, x2, x1	/* x2 = __bss_end - __bss_start */
	lsr	x2, x2, #3      /* x2 = x2 >> 3 ( x2 =x2 / 8 )  */
clear_bss_loop:
	cbz     w2, setup_bsp_stack
	str     xzr, [x1], #8
	sub     w2, w2, #1
	cbnz    w2, clear_bss_loop
setup_bsp_stack:
	/*
	 * setup stack for a boot processor
	 */
	ldr 	x30, =bsp_stack	
	add	x30, x30, STACK_SIZE
	sub	x30, x30, #TI_SIZE //Allocate ti size
	mov 	sp, x30
	str	xzr, [sp, #TI_PREEMPT_OFFSET]  /* Clear preempt info  */ 
	ldr	x0, =(THR_THREAD_INFO_MAGIC)  /*  Stack magic */
	str	x0, [sp, #TI_MAGIC_OFFSET]  
jmp_boot_main:	
	/*
	 * Jump to boot_main
	 */
	bl 	boot_main
	b .

