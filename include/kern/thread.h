/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  thread definitions                                                */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_THREAD_H)
#define _KERN_THREAD_H

#include "kern/kern_types.h"
#include "kern/list.h"
#include "kern/thread_info.h"

#include "hal/hal_types.h"
#include "hal/addrinfo.h"

/** スレッドの状態
 */
#define THR_TSTATE_RUN    (0)  /*< ランニング中 */
#define THR_TSTATE_WAIT   (1)  /*< 資源待ち中   */
#define THR_TSTATE_EXIT   (2)  /*< 終了中   */
#define THR_TSTATE_DEAD   (3)  /*< 終了待ち中   */

/** スレッド属性情報
 */
typedef struct _thread_attr{
	void         *stack_top;  /*< スタックの先頭アドレス                  */
	void             *stack;  /*< プロセススイッチ時のスタックポインタ    */
	size_t       stack_size;  /*< スタックサイズ                          */
	int                prio;  /*< 優先度                                  */
}thread_attr_t;

/** スレッド属性情報初期化子
    @param[in] stk      スタックの先頭アドレス
    @param[in] stk_size スタックのサイズ
 */
#define THR_ATTR_INITIALIZER(stk, stk_size) {			\
	.stack = TRUNCATE_ALIGN(stk + stk_size - sizeof(thread_info_t), stk_size), \
	.stack_size = stk_size,			\
	}

struct _thread_ready_queue;

/** スレッド管理情報
 */
typedef struct _thread{
	thr_state_t        status;  /*< スレッド状態                */
	tid_t                 tid;  /*< スレッドID                  */
	list_t               link;  /*< キューへのリンク            */
	list_t           mgr_link;  /*< スレッド管理用リンク        */
	struct _thread_ready_queue *rdq;  /*< レディーキュー              */
	exit_code_t     exit_code;  /*< 終了コード                  */
	thread_attr_t        attr;  /*< 属性情報                    */
	message_buffer_t  msg_buf;  /*< メッセージ送信情報          */
	wait_queue_t      recv_wq;  /*< メッセージ受信待ちキュー    */
	wait_queue_t      send_wq;  /*< メッセージ送信者待ちキュー  */
	list_head_t      recv_que;  /*< メッセージキュー            */
}thread_t;

/** スレッドがレディキューにつながっているか確認する
 */
#define THR_THREAD_ON_RDQ(thr) (thr->rdq != NULL)

/** スレッドキュー
 */
typedef struct _thread_queue{
	list_head_t head;    /*< スレッドキューのヘッド  */
}thread_queue_t;

thread_info_t *thr_refer_thread_info(thread_t *);
void thr_thread_switch(thread_t *, thread_t *);
void thr_unlink_thread(thread_t *);
int thr_create_thread(thread_t **, thread_attr_t *, void (*start)(void *), void *);
void thr_exit_thread(int );
int thr_destroy_thread(thread_t *);
void thr_thread_start(void (*)(void *), void   *);
tid_t thr_get_tid(thread_t *);
tid_t thr_get_current_tid(void);

void thr_init_thread_queue(thread_queue_t *);
void thr_add_thread_queue(thread_queue_t *, thread_t *);
void thr_remove_thread_queue(thread_queue_t *, thread_t *);
int thr_thread_queue_empty(thread_queue_t *);
thread_t *thr_thread_queue_get_top(thread_queue_t *);

int thr_can_receive_message(thread_t *);

static inline void
ti_update_preempt_count(thread_info_t *ti, uint64_t shift, int64_t diff) {
	psw_t psw;
	preempt_state_t	cnt;

	psw_disable_interrupt(&psw);

	cnt = ti->preempt;
	cnt = ( cnt >> shift ) & THR_CNTR_MASK;
	cnt = (preempt_state_t)( ( (int64_t)cnt ) + diff );
	cnt = ( cnt << shift ) & ( THR_CNTR_MASK << shift );

	ti->preempt &= ~( THR_CNTR_MASK << shift );
	ti->preempt |= cnt;
	
	psw_restore_interrupt(&psw);
}

static inline precnt_t
ti_refer_pre_count(thread_info_t *ti, int shift) {
	psw_t    psw;
	precnt_t cnt;

	psw_disable_interrupt(&psw);

	cnt = (precnt_t)( ( ti->preempt >> shift ) & THR_CNTR_MASK );

	psw_restore_interrupt(&psw);
	
	return cnt;
}

static inline precnt_t
ti_refer_preempt_count(thread_info_t *ti) {

	return ti_refer_pre_count(ti, THR_PRECNT_SHIFT);
}

static inline precnt_t
ti_refer_irq_count(thread_info_t *ti) {

	return ti_refer_pre_count(ti, THR_IRQCNT_SHIFT);
}

static inline precnt_t
ti_refer_exc_count(thread_info_t *ti) {

	return ti_refer_pre_count(ti, THR_EXCCNT_SHIFT);
}


#endif  /*  _KERN_THREAD_H */
