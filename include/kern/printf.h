/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  printf routines                                                   */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_PRINTF_H)
#define  _KERN_PRINTF_H

#include <kern/freestanding.h>

#define PRFBUFLEN        (128)  /*< Buffer length                */
int kvprintf(size_t , void *, char const *, int , va_list );
int kprintf(const char *, ...);
int ksnprintf(char *, size_t , const char *, ...);

#define panic(fmt, args ...)				\
	do{						\
		kprintf("panic: " fmt, ## args);	\
		while(1);				\
	}while(0)

#define kassert(cond) do {						\
	if ( !(cond) ) {                                                \
		kprintf("Assertion : [file:%s func %s line:%d ]\n",	\
		    __FILE__, __func__, __LINE__);			\
		while(1);                                               \
	}								\
	}while(0)
#endif  /*  _KERN_PRINTF_H   */
