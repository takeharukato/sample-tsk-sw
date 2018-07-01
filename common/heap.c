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
		return (void *)-1;

	kheap.cur += inc;

	return prev;
}

