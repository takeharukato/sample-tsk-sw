/* -*- mode: c; coding:utf-8 -*- */
/************************************************************************/
/*  OS kernel sample                                                    */
/*  Copyright 2014 Takeharu KATO                                        */
/*                                                                      */
/*  Some of size and offset information for assembler routines          */
/************************************************************************/
#define __IN_ASM_OFFSET 1

#include <kern/asm-offset-helper.h>
#include <kern/thread.h>

#include <hal/thread_info.h>
#include <hal/exception.h>

int
main(int  __attribute__ ((unused)) argc, char  __attribute__ ((unused)) *argv[]) {

	DEFINE_SIZE(TI_SIZE, sizeof(struct _thread_info));
	OFFSET(TI_PREEMPT_OFFSET, _thread_info, preempt);
	OFFSET(TI_MAGIC_OFFSET, _thread_info, magic);
	OFFSET(THREAD_TINFO_OFFSET, _thread, tinfo);

	DEFINE_SIZE(EXC_FRAME_SIZE, sizeof(struct _exception_frame));

	return 0;
}
