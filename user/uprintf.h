/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014-2018 Takeharu KATO                                 */
/*                                                                    */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_UPRINTF_H)
#define  _KERN_UPRINTF_H 

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

int uprintf(const char *, ...);
void cls(void);
void locate(int , int );

#endif  /*  _KERN_UPRINTF_H   */
