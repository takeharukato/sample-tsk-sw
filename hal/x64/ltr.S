/* -*- mode: gas; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2018 Takeharu KATO                                      */
/*                                                                    */
/*  Load Task State Segment region                                    */
/*                                                                    */
/**********************************************************************/

#define ASM_FILE   1

.code64
/* void ltr(uint16_t sel);  */
.global ltr
ltr:
	ltr    %di
	retq
	
