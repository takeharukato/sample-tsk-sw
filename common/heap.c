/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  kernel heap                                                       */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

#if defined(CONFIG_HAL)
//Need init with linker
static kernel_heap kheap = {
	.start = &_kheap_start,
	.end = &_kheap_end,
	.cur = &_kheap_start
};
#else
#include <stdlib.h>
#include <sys/mman.h>

#include <kern/param.h>

static kernel_heap kheap;

void
kheap_init(void){

	kheap.start = mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE, 
	    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if ( kheap.start == MAP_FAILED )
		abort();

	kheap.cur = kheap.start;
	kheap.end = kheap.start + HEAP_SIZE;
	
	return ;
}
#endif  /*  CONFIG_HAL  */
void *
kheap_sbrk(intptr_t inc){
	void *prev;

	prev = kheap.cur;
	if ( ( kheap.cur + inc < kheap.start ) || ( kheap.cur + inc >=  kheap.end ) )
		return HEAP_SBRK_FAILED;

	kheap.cur += inc;

	return prev;
}

mutex malloc_global_mutex=__MUTEX_INITIALIZER(malloc_global_mutex);

void 
kmalloc_initialize_lock(mutex *mtx){
	psw_t psw;	

	psw_disable_and_save_interrupt(&psw);
	mutex_init(mtx);
	psw_restore_interrupt(&psw);
}

void 
kmalloc_destroy_lock(mutex *mtx){
	psw_t psw;	

	psw_disable_and_save_interrupt(&psw);
	mutex_destroy(mtx);
	psw_restore_interrupt(&psw);
}

int 
kmalloc_try_lock(mutex *mtx){
	int rc;
	psw_t psw;	

	psw_disable_and_save_interrupt(&psw);
	rc = ( !mutex_try_hold(mtx) );
	psw_restore_interrupt(&psw);

	return rc;
}

int
kmalloc_acquire_lock(mutex *mtx){
	int    rc;
	psw_t psw;	
	
	psw_disable_and_save_interrupt(&psw);
	rc = mutex_hold(mtx);
	psw_restore_interrupt(&psw);

	return rc;
}

void
kmalloc_release_lock(mutex *mtx){
	psw_t psw;	

	psw_disable_and_save_interrupt(&psw);
	mutex_release(mtx);
	psw_restore_interrupt(&psw);
}
