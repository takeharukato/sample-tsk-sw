/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  ready queue definitions                                           */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_READY_QUE_H)
#define _KERN_READY_QUE_H

#include <kern/freestanding.h>

#include <kern/list.h>

#define RDQ_SYS_PRIORITY  (8)  /*< Priority of system threads  */
#define RDQ_USER_PRIORITY (8)  /*< Priority of user threads  */
#define RDQ_PRIORITY_MAX (RDQ_USER_PRIORITY + RDQ_SYS_PRIORITY)
#define RDQ_USER_RR_IDX   (0)  /*< Index for round robin threads    */

/** Ready queue
 */
typedef struct _thread_ready_queue{
	int                         bitmap;    /*< Bitmap for ready queue */
	list_head_t head[RDQ_PRIORITY_MAX];    /*< Thread queue head      */
}thread_ready_queue_t;

/** Calculate the priority from bitmap index
    @note IDX minus one means thread's priority since index zero means that
    no runnable thread exists.
 */
#define rdq_index2prio(idx) ((idx) - 1)

void rdq_add_thread(thread_t *);
void rdq_remove_thread(thread_t *);
void rdq_rotate_queue(void);
thread_t *rdq_find_runnable_thread(void);
void rdq_init_ready_queue(void);
#endif  /*  _KERN_READY_QUE_H  */
