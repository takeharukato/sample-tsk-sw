/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  wait queue definitions                                            */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_WQUEUE_H)
#define  _KERN_WQUEUE_H 
#include "kern/list.h"

struct _thread;

/** ウエイトキュー
 */
typedef struct _wait_queue{
	list_head_t head;   /*< 待ちキューのヘッド  */
}wait_queue_t;

typedef struct _wait_queue_entry{
	list_t          link;   /*< 待ちキューヘッドへのリンク   */
	struct _thread  *thr;   /*< スレッド管理情報へのポインタ */
}wait_queue_entry_t;


void wque_add_thread(wait_queue_t *, wait_queue_entry_t *, struct _thread  *);
void wque_remove_entry(wait_queue_t *, wait_queue_entry_t *);
void wque_init_wait_queue(wait_queue_t *);

void wque_wait_on_queue(wait_queue_t *);
void wque_wakeup(wait_queue_t *);
int  is_wque_empty(wait_queue_t *);
#endif  /*  _KERN_WQUEUE_H   */
