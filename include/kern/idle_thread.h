/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  idle thread                                                       */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_IDLE_THREAD_H)
#define  _KERN_IDLE_THREAD_H 
#include "kern/thread.h"

extern thread_t *current;  /*< カレントスレッドへのポインタ  */

void do_idle_loop(void);
thread_t *idle_refer_idle_thread(void);
void idle_init_idle(void);
#endif  /*  _KERN_IDLE_THREAD_H   */
