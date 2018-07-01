/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  String library in OS kernel                                       */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_STRING_H)
#define _KERN_STRING_H

#include <kern/freestanding.h>

#include <kern/errno.h>
#include <kern/kern_types.h>

void *memset(void *, int , size_t );
void *memcpy(void *, const void *, size_t );
void *memmove(void *, const void *, size_t );
size_t strlen(const char *);
#endif  /*  _KERN_STRING_H  */
