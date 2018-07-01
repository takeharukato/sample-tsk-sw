/* -*- mode: asm; coding:utf-8 -*- */
/************************************************************************/
/*  OS kernel sample                                                    */
/*  Copyright 2014 Takeharu KATO                                        */
/*                                                                      */
/*  start up data for aarch64                                           */
/************************************************************************/

#define ASM_FILE   1
#include <kern/param.h>

.data
.align 8
.global bsp_stack
bsp_stack:
	.skip STACK_SIZE
bsp_stack_end:
