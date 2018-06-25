/* -*- mode: asm; coding:utf-8 -*- */
/************************************************************************/
/*  OS kernel sample                                                    */
/*  Copyright 2014 Takeharu KATO                                        */
/*                                                                      */
/*  start up routines for aarch64                                       */
/************************************************************************/

#define ASM_FILE   1

#include <kern/param.h>

#include <hal/asm-offset.h>	
#include <hal/aarch64.h>

.global _start
start:	
_start:	
	msr     daifset, #(AARCH64_DAIF_FIQ | AARCH64_DAIF_IRQ) /* Disable FIQ and IRQ */

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
	 * Suspend cpus except cpuid == 0.
	 */
	mrs     x1, mpidr_el1  /* Read cpuid */
	and     x1, x1, #3     /*  Check whether CPUID is zero or not  */
	cbz     x1, 2f         /*  If CPUID is zero, continue to initialize */
1:
	wfe                    /*  Enter WFE(Wait For Event) loop if CPUID is NOT zero. */
	b       1b

2:
	/*
	 * setup stack for a boot processor
	 */
	ldr 	x30, =bsp_stack	
	add	x30, x30, STACK_SIZE
	sub	x30, x30, #TI_SIZE //Allocate ti size
	mov 	sp, x30
	bl 	boot_main
	b .

.data
.align 8
bsp_stack:
	.skip STACK_SIZE
bsp_stack_end:
