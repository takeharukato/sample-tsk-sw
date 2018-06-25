/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Exception definitions                                             */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_EXCEPTION_H)
#define  _HAL_EXCEPTION_H 

/*
 * AArch64 exception types
 */
#define AARCH64_EXC_SYNC_SP0      (0x1)   /*< Synchronous in EL0/SP0 */
#define AARCH64_EXC_IRQ_SP0       (0x2)   /*< IRQ in EL0/SP0 */
#define AARCH64_EXC_FIQ_SP0       (0x3)   /*< FIQ in EL0/SP0 */
#define AARCH64_EXC_SERR_SP0      (0x4)   /*< SError/vSError in EL1/SPx */
#define AARCH64_EXC_SYNC_SPX      (0x11)  /*< Synchronous in EL1/SPx */
#define AARCH64_EXC_IRQ_SPX       (0x12)  /*< IRQ in EL1/SPx */
#define AARCH64_EXC_FIQ_SPX       (0x13)  /*< FIQ in EL1/SPx */
#define AARCH64_EXC_SERR_SPX      (0x14)  /*< Synchronous in EL1/SPx */
#define AARCH64_EXC_SYNC_AARCH64  (0x21)  /*< Synchronous in EL1/SPx */
#define AARCH64_EXC_IRQ_AARCH64   (0x22)
#define AARCH64_EXC_FIQ_AARCH64   (0x23)
#define AARCH64_EXC_SERR_AARCH64  (0x24)
#define AARCH64_EXC_SYNC_AARCH32  (0x31)
#define AARCH64_EXC_IRQ_AARCH32   (0x32)
#define AARCH64_EXC_FIQ_AARCH32   (0x33)
#define AARCH64_EXC_SERR_AARCH32  (0x34)

#if defined(ASM_FILE)
#define vector_base_align .align 12  /* The vector base must be placed in 4096 bytes align */
#define vector_entry_align .align 7  /* Each vector entry must be placed in 256 bytes align */
#define text_align	.align	2    /* Text alignment */
#endif  /*  ASM_FILE  */

#if !defined(ASM_FILE)
#include <stdint.h>
typedef struct _exception_frame{
	uint64_t exc_type;
	uint64_t exc_esr;
	uint64_t exc_sp;
	uint64_t exc_elr;
	uint64_t exc_spsr;
	uint64_t x0;
	uint64_t x1;
	uint64_t x2;
	uint64_t x3;
	uint64_t x4;
	uint64_t x5;
	uint64_t x6;
	uint64_t x7;
	uint64_t x8;
	uint64_t x9;
	uint64_t x10;
	uint64_t x11;
	uint64_t x12;
	uint64_t x13;
	uint64_t x14;
	uint64_t x15;
	uint64_t x16;
	uint64_t x17;
	uint64_t x18;
	uint64_t x19;
	uint64_t x20;
	uint64_t x21;
	uint64_t x22;
	uint64_t x23;
	uint64_t x24;
	uint64_t x25;
	uint64_t x26;
	uint64_t x27;
	uint64_t x28;
	uint64_t x29;
	uint64_t x30;
}exception_frame;

void hal_common_trap_handler(exception_frame *_exc);
void hal_setup_vector(void);
#endif  /*  ASM_FILE  */
#endif  /*  _HAL_EXCEPTION_H   */
