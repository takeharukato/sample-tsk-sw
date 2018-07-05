/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Spinlock                                                          */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_SPINLOCK_H)
#define  _KERN_SPINLOCK_H 

#define SPINLOCK_TYPE_NORMAL     (0x0)  /*< Non recursive lock  */
#define SPINLOCK_TYPE_RECURSIVE  (0x1)  /*< Recursive lock      */

struct _thread_info;
typedef struct _spinlock {
	uint32_t            locked;  /*  Is the lock held?                                */
	uint32_t              type;  /*  lock type                                        */
	uint32_t               cpu;  /*  The cpu holding the lock.                        */
	uint32_t             depth;  /*  lock depth                                       */
	struct _thread_info *owner;  /*  lock owner thread info                           */
	uintptr_t backtrace[SPINLOCK_BT_DEPTH];       /*  back trace for debug            */
}spinlock;

#define __SPINLOCK_INITIALIZER		 \
	{				 \
		.locked = 0,		 \
		.type  = SPINLOCK_TYPE_NORMAL , \
		.cpu    = 0,             \
		.depth  = 0,             \
		.owner  = NULL,          \
	}

void spinlock_init(spinlock *_lock);
void spinlock_lock(spinlock *_lock);
void spinlock_unlock(spinlock *_lock);
void raw_spinlock_lock_disable_intr(spinlock *_lock, intrflags *_flags);
void raw_spinlock_unlock_restore_intr(spinlock *_lock, intrflags *_flags);
void spinlock_lock_disable_intr(spinlock *_lock, intrflags *_flags);
void spinlock_unlock_restore_intr(spinlock *_lock, intrflags *_flags);
bool spinlock_locked_by_self(spinlock *_lock);
bool check_recursive_locked(spinlock *_lock);

#endif  /*  _KERN_SPINLOCK_H   */
