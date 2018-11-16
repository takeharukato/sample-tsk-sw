/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014-2018 Takeharu KATO                                 */
/*                                                                    */
/*  Time Stamp Counter                                                */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_TSC_H)
#define  _HAL_TSC_H 
/** タイムスタンプカウンタを読み取る
 */
static inline uint64_t 
rdtsc(void) {
	unsigned long lo, hi;

        __asm__ __volatile__( "rdtsc" : "=a" (lo), "=d" (hi) ); 

        return( lo | (hi << 32) );
}

#endif  /*  _HAL_TSC_H   */
