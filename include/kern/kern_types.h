/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Type Definitions                                                  */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_KERN_TYPES_H)
#define _KERN_KERN_TYPES_H

#include <kern/freestanding.h>

#define BIT_PER_BYTE           (8)  /*<  We assume that the OS runs on byte machines */

typedef uint64_t         obj_id_t;  /*< ID                           */
typedef uint64_t      exit_code_t;  /*< Exit codes of threads        */
typedef int           thr_state_t;  /*< Status of a thread           */
typedef obj_id_t            tid_t;  /*< Thread ID                    */
typedef int32_t            irq_no;  /*< IRQ no                       */
typedef int32_t          irq_prio;  /*< IRQ priority                 */
typedef uint32_t         irq_attr;  /*< IRQ Attribute                */
typedef uint8_t          precnt_t;  /*< Preemption counters          */
typedef uint64_t       uptime_cnt;  /*< Jiffies                      */
struct _thread;
typedef obj_id_t           dev_id;  /*< Device ID                    */
typedef obj_id_t           blk_no;  /*< Dick block number            */
extern struct _thread *current;  /*< The pointer for the current thread  */
#endif  /*  _KERN_KERN_TYPES_H  */
