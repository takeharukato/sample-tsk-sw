/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  kernel mutex                                                      */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_MUTEX_H)
#define  _KERN_MUTEX_H 

#include <kern/freestanding.h>
#include <kern/thread.h>
#include <kern/wqueue.h>

typedef struct _mutex{
	uint32_t   count;  /*< counter    */
	thread_t *owner;   /*< Owner      */
	wait_queue   wq;   /*< wait queue */
}mutex;
#define __MUTEX_INITIALIZER(mtx) \
	{ 0, NULL, __WAIT_QUEUE_INITIALIZER(mtx.wq) }

void mutex_init(mutex *mtx);
void mutex_destroy(mutex *mtx);
int  mutex_try_hold(mutex *mtx);
int  mutex_hold(mutex *mtx);
void mutex_release(mutex *mtx);
#endif  /*  _KERN_MUTEX_H   */
