/* -*- mode: C; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  AArch64 definitions                                               */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_AARCH64_H)
#define  _HAL_AARCH64_H 

/*
 * Interrupt flags
 */
#define AARCH64_DAIF_FIQ    (1)
#define AARCH64_DAIF_IRQ    (2)
/*
 * Exception Level
 */
#define AARCH64_CUREL_MASK ( 0xc )  /* 2-3 bits        */
#define AARCH64_EL_EL3     ( 0xc )  /* Secure Monitor  */
#define AARCH64_EL_EL2     ( 0x8 )  /* Hyper Visor     */
#define AARCH64_EL_EL1     ( 0x4 )  /* OS              */
#define AARCH64_EL_EL0     ( 0x0 )  /* Application     */

/* SCR_EL3 
 * see 4.3.42. Secure Configuration Register in 
 * ARM Cortex-A53 MPCore Processor Technical Reference Manual
 */
#define AARCH64_SCR_EL3_NS   (0x1 << 0)    /* Non-secure bit  */
#define AARCH64_SCR_EL3_IRQ  (0x1 << 1)    /* Physical IRQ Routing */
#define AARCH64_SCR_EL3_FIQ  (0x1 << 2)    /* Physical FIQ Routing  */
#define AARCH64_SCR_EL3_EA   (0x1 << 3)    /* External Abort and SError interrupt Routing */
#define AARCH64_SCR_EL3_RES  (0x3 << 4)    /* Reserved */
#define AARCH64_SCR_EL3_SMA  (0x1 << 7)    /* SMC instruction disable */
#define AARCH64_SCR_EL3_HCE  (0x1 << 8)    /* Hyp Call enable */
#define AARCH64_SCR_EL3_SIF  (0x1 << 9)    /* Secure Instruction Fetch */
#define AARCH64_SCR_EL3_RW   (0x1 << 10)   /* Register width control for lower exception levels */
#define AARCH64_SCR_EL3_ST   (0x1 << 11)   /* Enable Secure EL1 access to CNTPS_TVAL_EL1, CNTS_CTL_EL1, and CNTPS_CVAL_EL1 registers */
#define AARCH64_SCR_EL3_TWI  (0x1 << 12)   /* Traps WFI instructions */
#define AARCH64_SCR_EL3_TWE  (0x1 << 13)   /* Traps WFE instructions */

/* HCR_EL2
 * see 4.3.36 Hypervisor Configuration Register in
 * ARM Cortex-A53 MPCore Processor Technical Reference Manual
 */
#define AARCH64_HCR_EL2_VM    (0x1 << 0)     /*  Enables second stage of translation  */
#define AARCH64_HCR_EL2_SWIO  (0x1 << 1)     /*  Set/Way Invalidation Override        */
#define AARCH64_HCR_EL2_PTW   (0x1 << 2)     /*  Protected Table Walk                 */
#define AARCH64_HCR_EL2_FMO   (0x1 << 3)     /*  Physical FIQ routing                 */
#define AARCH64_HCR_EL2_IMO   (0x1 << 4)     /*  Physical IRQ routing                 */
#define AARCH64_HCR_EL2_AMO   (0x1 << 5)     /*  Asynchronous abort and error interrupt routing  */
#define AARCH64_HCR_EL2_VF    (0x1 << 6)      /* Virtual FIQ interrupt */
#define AARCH64_HCR_EL2_VI    (0x1 << 7)      /* Virtual IRQ interrupt */
#define AARCH64_HCR_EL2_VSE   (0x1 << 8)      /* Virtual System Error/Asynchronous Abort */
#define AARCH64_HCR_EL2_FB    (0x1 << 9)      /* Forces broadcast */
#define AARCH64_HCR_EL2_BSU_NE    (0x0 << 10) /*  Barrier shareability upgrade(No effect) */
#define AARCH64_HCR_EL2_BSU_INNER (0x1 << 10) /*  Barrier shareability upgrade(Inner)     */
#define AARCH64_HCR_EL2_BSU_OUTER (0x2 << 10) /*  Barrier shareability upgrade(Outer)     */
#define AARCH64_HCR_EL2_BSU_FULL  (0x3 << 10) /*  Barrier shareability upgrade(Full)      */
#define AARCH64_HCR_EL2_DC        (0x1 << 12) /*  Default cacheable */
#define AARCH64_HCR_EL2_WFI       (0x1 << 13) /*  Traps WFI instruction if it causes suspension of execution */
#define AARCH64_HCR_EL2_WFE       (0x1 << 14) /*  Traps WFE instruction if it causes suspension of execution */
#define AARCH64_HCR_EL2_TID0      (0x1 << 15) /*  Traps ID group 0 registers */
#define AARCH64_HCR_EL2_TID1      (0x1 << 16) /*  Traps ID group 1 registers */
#define AARCH64_HCR_EL2_TID2      (0x1 << 17) /*  Traps ID group 2 registers */
#define AARCH64_HCR_EL2_TID3      (0x1 << 18) /*  Traps ID group 3 registers */
#define AARCH64_HCR_EL2_TSC       (0x1 << 19) /*  Traps SMC instruction */
#define AARCH64_HCR_EL2_TIDCP     (0x1 << 20) /*  Trap Implementation */
#define AARCH64_HCR_EL2_TACR      (0x1 << 21) /*  Traps Auxiliary Control registers */
#define AARCH64_HCR_EL2_TSW       (0x1 << 22) /*  Traps data or unified cache maintenance instructions by Set or Way */
#define AARCH64_HCR_EL2_TPC       (0x1 << 23) /*  Traps data or unified cache maintenance instructions to Point of Coherency */
#define AARCH64_HCR_EL2_TPU       (0x1 << 24) /*  Traps cache maintenance instructions to Point of Unification (POU). */
#define AARCH64_HCR_EL2_TTLB      (0x1 << 25) /*  Traps TLB maintenance instructions  */
#define AARCH64_HCR_EL2_TVM       (0x1 << 26) /*  Trap virtual memory controls  */
#define AARCH64_HCR_EL2_TGE       (0x1 << 27) /*  Traps general exceptions.  */
#define AARCH64_HCR_EL2_TDZ       (0x1 << 28) /*  Traps DC ZVA instruction.  */
#define AARCH64_HCR_EL2_HCD       (0x1 << 29) /*  Reserved  */
#define AARCH64_HCR_EL2_TRVM      (0x1 << 30) /*  Trap reads of Virtual Memory controls  */
#define AARCH64_HCR_EL2_RW        (0x1 << 31) /*  Register width control for lower exception levels.  */
#define AARCH64_HCR_EL2_CD        (0x1 << 32) /*   Disables stage 2 data cache  */
#define AARCH64_HCR_EL2_ID        (0x1 << 33) /*   Disables stage 2 instruction cache.  */
/* Saved Process Status Register
 * See 4.1.5 Saved Process Status Register in
 * ARM  Cortex-A Series Programmer’s Guide for ARMv8-A
 */
#define AARCH64_SPSR_DAIF_F_BIT   (0x1 << 6)  /*  FIQ interrupt Process state mask */
#define AARCH64_SPSR_DAIF_I_BIT   (0x1 << 7)  /*  IRQ interrupt Process state mask */
#define AARCH64_SPSR_DAIF_A_BIT   (0x1 << 8)  /*  SError interrupt Process state mask */
#define AARCH64_SPSR_DAIF_D_BIT   (0x1 << 9)  /*  Debug exceptions mask */
#define AARCH64_SPSR_FROM_AARCH64 (0x0 << 4)  /*  Exception was taken from AArch64  */
#define AARCH64_SPSR_FROM_AARCH32 (0x1 << 4)  /*  Exception was taken from AArch64  */
#define AARCH64_SPSR_MODE_EL3     (AARCH64_EL_EL3)       /*  Exception State EL3 */
#define AARCH64_SPSR_MODE_EL2     (AARCH64_EL_EL2)       /*  Exception State EL2 */
#define AARCH64_SPSR_MODE_EL1     (AARCH64_EL_EL1)       /*  Exception State EL1 */
#define AARCH64_SPSR_MODE_EL0     (AARCH64_EL_EL0)       /*  Exception State EL0 */
#define AARCH64_SPSR_SP_SEL_N     (0x1 << 0)
#define AARCH64_SPSR_SP_SEL_0     (0x0 << 0)



#if !defined(ASM_FILE)
#include <stdint.h>

static inline uint64_t
aarch64_current_el(void) {
	uint64_t curel;

	__asm__ __volatile__ ("mrs %0, CurrentEL\n\t":"=r"(curel));
	curel &= AARCH64_CUREL_MASK;
	curel >>= 2;
	return curel;
}

/* Generic Timer register operations
 * See D10.6 Generic Timer registers in
 * ARM Architecture Reference Manual ARMv8, for ARMv8-A architecture profile
 */

static inline uint64_t
aarch64_get_physical_counter(void) {
	uint64_t val;

	__asm__ __volatile__("mrs %0, cntpct_el0\n\t" : "=r"(val));

	return val;
}

static inline uint64_t
aarch64_get_generic_timer_freq(void) {
	uint64_t val;

	__asm__ __volatile__("mrs %0, cntfrq_el0\n\t" : "=r"(val));

	return val;
}

void aarch64_setup_vector(void);
#endif  /*  !ASM_FILE  */
#endif  /*  _HAL_AARCH64_H   */
