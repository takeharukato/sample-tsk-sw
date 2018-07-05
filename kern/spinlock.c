/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Spinlock dummy                                                    */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>
void 
spinlock_init(spinlock *lock){
}
void 
spinlock_lock(spinlock *lock){
}
void 
spinlock_unlock(spinlock *lock){
}
void 
raw_spinlock_lock_disable_intr(spinlock *lock, intrflags *flags){
}
void 
raw_spinlock_unlock_restore_intr(spinlock *lock, intrflags *flags){
}
void 
spinlock_lock_disable_intr(spinlock *lock, intrflags *flags){
}
void
spinlock_unlock_restore_intr(spinlock *lock, intrflags *flags){
}
bool 
spinlock_locked_by_self(spinlock *lock){
}
bool 
check_recursive_locked(spinlock *lock){
}
