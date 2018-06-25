/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Thread Information                                                */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_THREAD_INFO_H)
#define  _KERN_THREAD_INFO_H 

/** Magic nunber which indicate the end of stack  */
#define THR_THREAD_INFO_MAGIC                  (0xdeadbeef)  

/** Delay dispatching 
 */
#define THR_DISPATCH_ENABLE   (0x0)  /*< Dispatch is enabled       */
#define THR_DISPATCH_DISABLE  (0x1)  /*< Dispatch is NOT enabled       */
#define THR_DISPATCH_DELAYED  (0x2)  /*< Dispatch requests are delayed */
#define THR_DISPATCH_MASK  (THR_DISPATCH_DISABLE|THR_DISPATCH_DELAYED) /*< mask for dispatch state */

#if !defined(ASM_FILE)
#include <stdint.h>

typedef uint64_t  preempt_state_t;  /*< dispatch state  */
/** Thread info in kernel stack
 */
typedef struct _thread_info{
	preempt_state_t	preempt;   /*< Dispatch state  */
	uintptr_t         magic;     /*< Magic number */
}thread_info_t;

#endif  /*  ASM_FILE  */
#endif  /*  _KERN_THREAD_INFO_H   */
