/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Timer definitions                                                 */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_TIMER_H)
#define  _KERN_TIMER_H 

#include <kern/freestanding.h>
#include <kern/kern_types.h>

typedef struct _uptime_counter{
	uptime_cnt  uptime;
}uptime_counter;

void timer_update_uptime(void);
void timer_update_thread_time(void);
#endif  /*  _KERN_TIMER_H   */
