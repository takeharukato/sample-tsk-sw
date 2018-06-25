/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  thread manager definitions                                        */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_THREAD_MANAGER_H)
#define _KERN_THREAD_MANAGER_H
#include "kern/kern_types.h"
#include "kern/list.h"
#include "hal/hal_types.h"
#include "hal/addrinfo.h"

/** スレッド管理構造
 */
typedef struct _thread_manager{
	list_head_t    head;    /*< スレッド管理構造リストのヘッド  */
}thread_manager_t;

int thrmgr_get_threadid(tid_t *);
void thrmgr_put_threadid(tid_t );
int thrmgr_reserve_threadid(tid_t );

void thrmgr_init_thread_manager(void);
thread_manager_t *thrmgr_refer_thread_manager(void);
void thrmgr_thread_manager_add(thread_manager_t *, thread_t *);
void thrmgr_thread_manager_remove(thread_manager_t *, thread_t *);
int thrmgr_find_thread_by_tid(tid_t , thread_t **);

#endif  /*  _KERN_THREAD_MANAGER_H  */
