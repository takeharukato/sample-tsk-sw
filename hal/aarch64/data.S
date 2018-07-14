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

.p2align 12
.global pre_ttbr0_l1tbl
pre_ttbr0_l1tbl:
    .zero   PAGE_SIZE

.p2align 12
.global pre_ttbr0_l2tbl
pre_ttbr0_l2tbl:
    .zero   PAGE_SIZE

.p2align 12
.global pre_ttbr1_l1tbl
pre_ttbr1_l1tbl:
    .zero   PAGE_SIZE

.p2align 12
.global pre_ttbr1_l2tbl
pre_ttbr1_l2tbl:
    .zero   PAGE_SIZE
