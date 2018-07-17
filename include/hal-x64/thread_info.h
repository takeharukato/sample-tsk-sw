/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Thread Information                                                */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_THREAD_INFO_H)
#define  _HAL_THREAD_INFO_H 

/** Magic nunber which indicate the end of stack  */
#define THR_THREAD_INFO_MAGIC                  (0xdeadbeef)  

/** Preempt/IRQ Count/Exception Count/Delay dispatching 
 */
#define THR_PRMPT_CTRL_SHIFT  (56)   /*< Shift value for Preempt control flags */
#define THR_DISPATCH_SHIFT    (24)   /*< Shift value for dispatch flags */
#define THR_PRECNT_SHIFT      (16)   /*< Shift value for Preempt count  flags */
#define THR_IRQCNT_SHIFT      (8)    /*< Shift value for IRQ count */
#define THR_EXCCNT_SHIFT      (0)    /*< Shift value for Exeception count */

#if !defined(ASM_FILE)

#include <kern/freestanding.h>
#include <hal/addrinfo.h>
#include <hal/psw.h>

#define THR_CNTR_MASK         (0xffULL) /*< Mask value for counters */
#define THR_PRMPT_CTRL_BITS   (0xffULL) /*< Reserved mask value for counters */
/** Dispatch requests are delayed */
#define THR_PRMPT_CTRL_PREEMPT_ACTIVE    ((0x80ULL) << THR_PRMPT_CTRL_SHIFT)  
/** Dispatch requests are delayed */
#define THR_PRMPT_CTRL_RESCHED_DELAYED   ((0x40ULL) << THR_PRMPT_CTRL_SHIFT)  
/** mask for dispatch state */
#define THR_PRMPT_CTRL_MASK     \
	(THR_PRMPT_CTRL_PREEMPT_ACTIVE|THR_PRMPT_CTRL_RESCHED_DELAYED) 

typedef uint64_t  preempt_state_t;  /*< dispatch state  */
/** Thread info in kernel stack
 */
struct _thread;
typedef struct _thread_info{
	preempt_state_t	preempt;   /*< Dispatch state  */
	struct _thread     *thr;   /*< Thread info     */
	uint64_t            pad;   /*< padding         */
	uintptr_t         magic;   /*< Magic number    */
}thread_info_t;

/** 遅延ディスパッチ予約を立てる
    @param[in] tinfo スレッド情報のアドレス
 */
static inline void
ti_set_delay_dispatch(thread_info_t *tinfo) {

	tinfo->preempt |= THR_PRMPT_CTRL_RESCHED_DELAYED;
}

/** 遅延ディスパッチ予約をクリアする
    @param[in] tinfo スレッド情報のアドレス
 */
static inline void
ti_clr_delay_dispatch(thread_info_t *tinfo) {

	tinfo->preempt &= ~THR_PRMPT_CTRL_RESCHED_DELAYED;
}

/** 遅延ディスパッチ予約があることを確認する
    @param[in] tinfo スレッド情報のアドレス
 */
static inline int
ti_check_need_dispatch(thread_info_t *tinfo) {

	return ( ( tinfo->preempt & THR_PRMPT_CTRL_RESCHED_DELAYED ) != 0 );
}

/** スケジュール実行中フラグを立てる
    @param[in] tinfo スレッド情報のアドレス
 */
static inline void
ti_set_preempt_active(thread_info_t *tinfo) {

	tinfo->preempt |= THR_PRMPT_CTRL_PREEMPT_ACTIVE;
}

/** スケジュール実行中フラグをクリアする
    @param[in] tinfo スレッド情報のアドレス
 */
static inline void
ti_clr_preempt_active(thread_info_t *tinfo) {

	tinfo->preempt &= ~THR_PRMPT_CTRL_PREEMPT_ACTIVE;
}

static inline void *
hal_get_sp(void) {
	uint64_t sp;

	__asm__ __volatile__("mov %%rsp, %0" : "=r" (sp));  

	return (void *)sp;
}

#endif  /*  ASM_FILE  */
#endif  /*  _HAL_THREAD_INFO_H   */
