/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Some support macros for assembler codes                           */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_ASM_MACROS_H)
#define  _HAL_ASM_MACROS_H 

#if defined(ASM_FILE)

#define UI_C(val)    (val)         /*< unsigined int constant in asm */
#define UL_C(val)    (val)         /*< unsigned long constant in asm */
#define ULL_C(val)   (val)         /*< unsigned long long constant in asm */

#define vector_base_align  .balign  0x800 /*< The vector base must be placed in 
					    2048 bytes aligned address because 
					    0-10 bits in VBAR_ELx are reserved as zero.
					    See D10.2.116 VBAR_EL1, 
					    Vector Base Address Register (EL1)
					    in ARM Architecture Reference Manual ARMv8,
					    for ARMv8-A architecture profile.  */
#define vector_entry_align .balign  0x80   /*< Each vector entry must be placed in 128 bytes align */
#define text_align	   .balign  0x4    /*< Text alignment */

#else  /* !ASM_FILE */
#define UI_C(val)     (val ## U)           /*< unsigined int constant in C */
#define UL_C(val)    (val ## UL)           /*< unsigned long constant in C */
#define ULL_C(val)    (val ## ULL)         /*< unsigned long long constant in C */
#endif  /*  ASM_FILE  */
#endif  /*  _HAL_ASM_MACROS_H   */
