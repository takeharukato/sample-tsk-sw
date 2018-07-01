/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  AArch64 Virtual Timer Definitions                                 */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_TIMER_H)
#define  _HAL_TIMER_H 

#include <kern/freestanding.h>
#include <kern/kern_types.h>

#include <hal/exception.h>
#include <hal/board.h>

void hal_init_timer(uint64_t _timer_interval);
#endif  /*  _HAL_TIMER_H   */
