/* -*- mode: asm; coding:utf-8 -*- */
/************************************************************************/
/*  OS kernel sample                                                    */
/*  Copyright 2014 Takeharu KATO                                        */
/*                                                                      */
/*  File system image                                                   */
/*                                                                      */
/************************************************************************/

.data
	
.balign  4096
.global	_fsimg_start
_fsimg_start:	
	.incbin "../../fsimg.img"
.balign  4096
.global	_fsimg_end
_fsimg_end:
	 .byte 0

