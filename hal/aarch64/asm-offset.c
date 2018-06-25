/* -*- mode: c; coding:utf-8 -*- */
/************************************************************************/
/*  OS kernel sample                                                    */
/*  Copyright 2014 Takeharu KATO                                        */
/*                                                                      */
/*  Some of size and offset information for assembler routines          */
/************************************************************************/
#include <kern/asm-offset-helper.h>
#include <kern/thread_info.h>

#define __IN_ASM_OFFSET 1

int
main(int  __attribute__ ((unused)) argc, char  __attribute__ ((unused)) *argv[]) {

	DEFINE_SIZE(TI_SIZE, sizeof(struct _thread_info));
	OFFSET(TI_FLAGS_OFFSET, _thread_info, preempt);
	OFFSET(TI_FLAGS_MAGIC, _thread_info, magic);
}
