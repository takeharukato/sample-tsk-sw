/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Kernel heap                                                       */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_HEAP_H)
#define  _KERN_HEAP_H 

#include <kern/freestanding.h>
#include <kern/mutex.h>

typedef struct _kernel_heap{
	void *start;  /*< Start address of kernel heap */
	void *end;    /*< End address of kernel heap */
	void *cur;    /*< Current heap pointer */
}kernel_heap;

extern void *_kheap_start;
extern void *_kheap_end;

extern mutex malloc_global_mutex;

void *kheap_sbrk(intptr_t _inc);
void kheap_init(void);

void kmalloc_initialize_lock(mutex *mtx);
void kmalloc_destroy_lock(mutex *mtx);
int kmalloc_try_lock(mutex *mtx);
int kmalloc_acquire_lock(mutex *mtx);
void kmalloc_release_lock(mutex *mtx);

#endif  /*  _KERN_HEAP_H   */
