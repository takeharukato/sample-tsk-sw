/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  reaper thread                                                     */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_REAPER_H)
#define  _KERN_REAPER_H 
#include "kern/thread.h"
#include "kern/wqueue.h"

#define REAPER_THREAD_TID  (1)  /*< 回収スレッドのスレッドID  */
#define REAPER_THREAD_PRIO (7)  /*< 回収スレッドの優先度      */
typedef struct _reaper_thread_info{
	thread_queue_t reaper_queue;  /*< 回収対象のスレッド  */
	wait_queue_t             wq;  /*< 回収要求待ちキュー  */
}reaper_thread_info_t;

void reaper_add_exit_thread(thread_t *);
void reaper_init_thread(void);
#endif  /*  _KERN_REAPER_H   */
