/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Reaper Thread                                                     */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_REAPER_H)
#define  _KERN_REAPER_H 

#include <kern/freestanding.h>

#include <kern/thread.h>
#include <kern/wqueue.h>

#define REAPER_THREAD_TID  (1)  /*< Thread ID of the reaper thread  */
#define REAPER_THREAD_PRIO (7)  /*< Priority of the reaper thread  */
typedef struct _reaper_thread_info{
	thread_queue_t reaper_queue;  /*< Threads to be ended  */
	wait_queue               wq;  /*< Wait queue to recieve terminate requests  */
}reaper_thread_info_t;

void reaper_add_exit_thread(thread_t *);
void reaper_init_thread(void);
#endif  /*  _KERN_REAPER_H   */
