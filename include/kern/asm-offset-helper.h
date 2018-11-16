/* -*- mode: C; coding:utf-8 -*- */
/***********************************************************************/
/*  OS kernel sample                                                   */
/*  Copyright 2014 Takeharu KATO                                       */
/*                                                                     */
/*  Definitions for calculation of offset addresses in structured data */
/*                                                                     */
/***********************************************************************/
#if !defined(_KERN_ASM_OFFSET_HELPER_H)
#define _KERN_ASM_OFFSET_HELPER_H

#include <kern/freestanding.h>

#define __asm_offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/** Define structure size
   @param[in] sym macro name
   @param[in] val size of structured data(e.g., sizeof(struct _xxxx))
 */
#define DEFINE_SIZE(sym, val) 						  \
        __asm__ __volatile__ ("\n.ascii \"@ASM_OFFSET@" #sym " %0 " #val "\"" : : "i" (val)) \

/** Define offset address
   @param[in] sym macro name
   @param[in] str name of the structured data
   @param[in] member name in the structured data   
 */
#define OFFSET(sym, str, mem) \
	DEFINE_SIZE(sym, __asm_offsetof(struct str, mem))
#endif  /*  _KERN_ASM_OFFSET_HELPER_H  */
