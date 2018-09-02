/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Kernel map                                                        */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_KMAP_H)

#include <kern/param.h>
#include <hal/asm-macros.h>

#define KERN_VMA_BASE               (ULL_C(0x0000000000000000))
#define PHY_TO_KERN_STRAIGHT(phy)   ( (phy) + KERN_VMA_BASE  )
#define KERN_STRAIGHT_TO_PHY(vaddr) ( (vaddr) - KERN_VMA_BASE)
#define KERN_STRAIGHT_PAGE_SIZE     (CONFIG_HAL_STRAIGHT_PAGE_SIZE)

#if !defined(ASM_FILE)

#include <kern/freestanding.h>

#include <hal/aarch64.h>
#include <hal/board.h>
#include <hal/pgtbl.h>

#define KERN_STRAIGHT_PAGE_START(addr)					\
	( (uintptr_t)( ( (uintptr_t)(addr) ) &				\
	    ~( (uintptr_t)(KERN_STRAIGHT_PAGE_SIZE) - (uintptr_t)1 ) ) )

#define KERN_STRAIGHT_PAGE_NEXT(addr)			  \
	( (uintptr_t)KERN_STRAIGHT_PAGE_START( (addr) ) + \
	    (uintptr_t)(KERN_STRAIGHT_PAGE_SIZE) )

#define KERN_STRAIGHT_PAGE_END(addr)		\
	( KERN_STRAIGHT_PAGE_NEXT((addr)) - 1 )

void map_kernel_page(uintptr_t _paddr, uintptr_t _vaddr, uint64_t _page_attr, void *_kpgtbl);
void aarch64_setup_mmu(void);
#endif  /*  ASM_FILE  */
#endif  /*  _HAL_KMAP_H   */
