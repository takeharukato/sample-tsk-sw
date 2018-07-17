/* -*- mode: gas; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2018 Takeharu KATO                                      */
/*                                                                    */
/*  Load interrupt descriptor table region                            */
/*                                                                    */
/**********************************************************************/

#define ASM_FILE   1

.code64
/* void lidtr(struct region_descriptor *rdp);  */
.global lidtr
lidtr:
	lidt    (%rdi)
	retq
	
