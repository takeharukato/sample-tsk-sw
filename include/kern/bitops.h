/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  bit operations                                                    */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_BITOPS_H)
#define _KERN_BITOPS_H

#include <kern/freestanding.h>

#include <kern/kern_types.h>

int  find_msr_bit(int , int *);
int  find_lsr_bit(int , int *);
#endif  /*  _KERN_BITOPS_H  */
