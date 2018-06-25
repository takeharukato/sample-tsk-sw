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
#include <stdarg.h>

#define PRFBUFLEN        (128)  /*< 内部バッファ長                    */
int kprintf(const char *, ...);
#endif  /*  _KERN_PRINTF_H   */
