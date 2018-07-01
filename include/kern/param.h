/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Kernel parameters                                                 */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_PARAM_H)
#define _KERN_PARAM_H

#include <kern/autoconf.h>

#define HEAP_SIZE       ((CONFIG_HEAP_SIZE)*1024*1024)
#define STACK_SIZE      ((CONFIG_KSTACK_PAGE_NR)*(CONFIG_HAL_PAGE_SIZE))
#endif  /*   _KERN_PARAM_H  */
