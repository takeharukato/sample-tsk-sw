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
int strcmp(char const *_s1, char const *_s2);
int strncmp(const char *_s1, const char *_s2, size_t _n);
char *strcpy(char *_dest, char const *_src);
char *strncpy(char *_dest, char const *_src, size_t _count);
#endif  /*  _KERN_STRING_H  */
