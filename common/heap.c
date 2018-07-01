/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  kernel heap                                                       */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

//Need init with linker
static kernel_heap kheap = {
	.start = &_kheap_start,
	.end = &_kheap_end,
	.cur = &_kheap_start
};

void *
kheap_sbrk(intptr_t inc){
	void *prev;

	prev = kheap.cur;
	if ( ( kheap.cur + inc < kheap.start ) || ( kheap.cur + inc >=  kheap.end ) )
		return (void *)-1;

	kheap.cur += inc;

	return prev;
}
