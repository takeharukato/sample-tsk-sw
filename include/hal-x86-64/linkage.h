/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  アセンブラ関連マクロ                                              */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_LINKAGE_H)
#define  _HAL_LINKAGE_H 

#define _START_ENTRY    \
	.text;		\
	.p2align 4,0x90

#define ENTRY(x)		      \
	_START_ENTRY;		      \
	.globl x;		      \
	.type x,@function; x:

#define END(x)          .size x, . - x

#define __CPP_CONCAT(x,y) x ## y

#define IVEC(name) \
	__CPP_CONCAT(idtvec_,name)

#define IDTVEC(name)   .p2align 4,0x90;					\
	.globl IVEC(name);					\
	.type IVEC(name),@function;				\
        IVEC(name):
#endif  /*  _HAL_LINKAGE_H   */
