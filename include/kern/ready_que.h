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

#define RDQ_SYS_PRIORITY  (7)  /*< システムスレッドの優先度  */
#define RDQ_USER_PRIORITY (1)  /*< ユーザスレッドの優先度    */
#define RDQ_PRIORITY_MAX (RDQ_USER_PRIORITY + RDQ_SYS_PRIORITY)
#define RDQ_USER_QUE_IDX  (0)  /*< ユーザスレッドのインデクス    */
/** レディーキュー
 */
typedef struct _thread_ready_queue{
	int                         bitmap;    /*< レディーキューのビットマップ  */
	list_head_t head[RDQ_PRIORITY_MAX];    /*< スレッドキューのヘッド        */
}thread_ready_queue_t;

/** ビットマップのインデクスから優先度を算出する
    @note ビットマップのインデクスは, 動作可能スレッドなしを0で表すために,
    1からつけられているので, -1して優先度を算出する
 */
#define rdq_index2prio(idx) ((idx) - 1)

void rdq_add_thread(thread_ready_queue_t *, thread_t *);
void rdq_remove_thread(thread_ready_queue_t *, thread_t *);
void rdq_rotate_queue(thread_ready_queue_t *);
thread_t *rdq_find_runnable_thread(thread_ready_queue_t *);
void rdq_init_ready_queue(thread_ready_queue_t *);
#endif  /*  _KERN_READY_QUE_H  */
