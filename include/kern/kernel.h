/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  kernel common header                                              */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_KERNEL_H)
#define _KERN_KERNEL_H
#include "kern/kern_types.h"
#include "kern/string.h"
#include "kern/bitops.h"
#include "kern/list.h"
#include "kern/kconsole.h"
#include "kern/printf.h"
#include "kern/malloc.h"
#include "kern/id_bitmap.h"
#include "kern/wqueue.h"
#include "kern/message.h"
#include "kern/thread.h"
#include "kern/thread_manager.h"
#include "kern/ready_que.h"
#include "kern/reaper.h"
#include "kern/idle_thread.h"
#include "kern/sched.h"
#include "kern/main.h"

#include "hal/hal.h"

#endif  /*  _KERN_KERNEL_H  */
