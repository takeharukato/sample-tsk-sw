/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014-2018 Takeharu KATO                                 */
/*                                                                    */
/*  Machine Specific Register                                         */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_MSR_H)
#define  _HAL_MSR_H
#if !defined(ASM_FILE)
#include <kern/freestanding.h>

static inline void
wrmsr(uint32_t msr_id, uint64_t msr_value){

	asm volatile ( "wrmsr" : : "c" (msr_id), "A" (msr_value) );
}

static inline uint64_t 
rdmsr(uint32_t msr_id) {
	uint64_t msr_value;

	asm volatile ( "rdmsr" : "=A" (msr_value) : "c" (msr_id) );

	return msr_value;
}
#endif  /*  !ASM_FILE  */
#endif  /*  _HAL_MSR_H   */
