/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Wait queue definitions                                            */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_WQUEUE_H)
#define  _KERN_WQUEUE_H 

#include <kern/freestanding.h>

#include <kern/list.h>

struct _thread;

typedef uint32_t wq_reason;  /*< Type for the reason why threads were awaken.  */

#define WQUE_REASON_WAKEUP  (0x0)  /*< wque_wakeup is called */
#define WQUE_REASON_DESTROY (0x1)  /*< The object is destroyed */

/** Wait queue
 */
typedef struct _wait_queue{
	list_head_t head;   /*< Wait queue head  */
	wq_reason   reason; /*< Wakeup reason    */
}wait_queue;

typedef struct _wait_queue_entry{
	list_t          link;   /*< a link for wait queue head  */
	struct _thread  *thr;   /*< a pointer to a slept thread */
}wait_queue_entry;

#define __WAIT_QUEUE_INITIALIZER(wq)  \
	{__LIST_HEAD_INITIALIZER(wq.head)}

void wque_add_thread(wait_queue *_wque, wait_queue_entry *_ent, struct _thread  *_thr);
void wque_remove_entry(wait_queue *_wque, wait_queue_entry *_ent);
void wque_init_wait_queue(wait_queue *_wque);
wq_reason wque_wait_on_queue(wait_queue *_wque);
void wque_wakeup(wait_queue *_wque, wq_reason _reason);
int  is_wque_empty(wait_queue *_wque);
#endif  /*  _KERN_WQUEUE_H   */
