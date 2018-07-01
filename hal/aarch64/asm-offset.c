/* -*- mode: c; coding:utf-8 -*- */
/************************************************************************/
/*  OS kernel sample                                                    */
/*  Copyright 2014 Takeharu KATO                                        */
/*                                                                      */
/*  Some of size and offset information for assembler routines          */
/************************************************************************/
#define __IN_ASM_OFFSET 1

#include <kern/asm-offset-helper.h>
#include <kern/thread_info.h>

#include <hal/exception.h>

int
main(int  __attribute__ ((unused)) argc, char  __attribute__ ((unused)) *argv[]) {

	DEFINE_SIZE(TI_SIZE, sizeof(struct _thread_info));
	OFFSET(TI_PREEMPT_OFFSET, _thread_info, preempt);
	OFFSET(TI_MAGIC_OFFSET, _thread_info, magic);

	DEFINE_SIZE(EXC_FRAME_SIZE, sizeof(struct _exception_frame));
	OFFSET(EXC_EXC_TYPE_OFFSET, _exception_frame, exc_type);
	OFFSET(EXC_EXC_ESR_OFFSET, _exception_frame, exc_esr);
	OFFSET(EXC_EXC_SP_OFFSET, _exception_frame, exc_sp);
	OFFSET(EXC_EXC_ELR_OFFSET, _exception_frame, exc_elr);
	OFFSET(EXC_EXC_SPSR_OFFSET, _exception_frame, exc_spsr);
}
