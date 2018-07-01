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

int  find_msr_bit_in_size(size_t );
int  find_lsr_bit_in_size(size_t );
int  find_msr_bit(int );
int  find_lsr_bit(int );
#endif  /*  _KERN_BITOPS_H  */
