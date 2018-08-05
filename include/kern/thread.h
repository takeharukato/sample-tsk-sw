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

#include <kern/freestanding.h>

#include <kern/kern_types.h>
#include <kern/list.h>

#include <hal/thread_info.h>
#include <hal/hal_types.h>
#include <hal/addrinfo.h>

/** Thread  status
 */
#define THR_TSTATE_RUN    (0)  /*< Running */
#define THR_TSTATE_WAIT   (1)  /*< Waiting for some resources   */
#define THR_TSTATE_EXIT   (2)  /*< Exiting   */
#define THR_TSTATE_DEAD   (3)  /*< Wait for termination   */

#define THR_FDS_NR        (32) /*< Open files */

/** Thread attributes
 */
typedef struct _thread_attr{
	void         *stack_top;  /*< Start address of the stack                   */
	void             *stack;  /*< Stack pointer when the thread were switched  */
	size_t       stack_size;  /*< Stack size                                   */
	int                prio;  /*< Thread priority                              */
}thread_attr_t;

/** A macro to initialize a thread attribute
    @param[in] stk      Start address of the stack
    @param[in] stk_size Stack size
 */
#define THR_ATTR_INITIALIZER(stk, stk_size) {			\
	.stack = TRUNCATE_ALIGN(stk + stk_size - sizeof(thread_info_t), stk_size), \
	.stack_size = stk_size,			\
	}

/** Thread definiion
 */
struct _file_descriptor;
typedef struct _thread{
	thr_state_t        status;  /*< Status of the thread       */
	tid_t                 tid;  /*< Thread ID                  */
	list_t               link;  /*< link for thread queue      */
	thread_info_t      *tinfo;  /*< link for thread info       */
	exit_code_t     exit_code;  /*< Exit code                  */
	thread_attr_t        attr;  /*< Thread attribute           */
	uptime_cnt          slice;  /*< Remaining time slices      */
	struct _file_descriptor *fds[THR_FDS_NR]; /*< File descriptors  */
}thread_t;

/** Thread queue
 */
typedef struct _thread_queue{
	list_head_t head;    /*< Thread queue head  */
}thread_queue_t;

void thr_thread_switch(thread_t *, thread_t *);
void thr_unlink_thread(thread_t *);
int thr_create_thread(tid_t ,thread_t **, thread_attr_t *, void (*start)(void *), void *);
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

thread_info_t *thr_refer_thread_info(thread_t *);

static inline void
ti_update_preempt_count(thread_info_t *ti, uint64_t shift, int64_t diff) {
	psw_t psw;
	preempt_state_t	cnt;

	psw_disable_and_save_interrupt(&psw);

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

	psw_disable_and_save_interrupt(&psw);

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

/** 現在のスレッドのスレッド情報を得る
 */
static inline thread_info_t *
get_current_thread_info(void) {
	return current->tinfo;
}

#endif  /*  _KERN_THREAD_H */
