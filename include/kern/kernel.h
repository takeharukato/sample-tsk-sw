/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  kernel common header                                              */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_KERNEL_H)
#define _KERN_KERNEL_H

#include <kern/freestanding.h>

#include <kern/param.h>
#include <kern/kern_types.h>
#include <kern/errno.h>
#include <kern/string.h>
#include <kern/bitops.h>
#include <kern/list.h>
#include <kern/kconsole.h>
#include <kern/printf.h>
#include <kern/malloc.h>
#include <kern/wqueue.h>
#include <kern/thread.h>
#include <kern/ready_que.h>
#include <kern/reaper.h>
#include <kern/idle_thread.h>
#include <kern/sched.h>
#include <kern/irq.h>
#include <kern/main.h>
#include <kern/irq.h>
#include <kern/mutex.h>
#include <kern/heap.h>
#include <kern/timer.h>
#include <kern/blkio.h>
#include <kern/fdtable.h>
#include <kern/fs.h>

#include <hal/hal.h>

#endif  /*  _KERN_KERNEL_H  */
