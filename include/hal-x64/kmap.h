/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Kernel map                                                        */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_KMAP_H)

#include <hal/x64.h>

#define KERN_HIGH_IO_AREA         (0xFE000000)
#define KERN_HIGH_MEMORY_BASE     (0x100000000)
#define KERN_PHY_BASE             (0x0000000000000000)
#define KERN_PHY_MIN              (0x0000000000000000)  /* 0 */
#define KERN_PHY_MAX              (0x0000000040000000)  /* 1GiB */

#define KERN_VMA_BASE             (0xFFFF800000000000)                                 
#define KERN_HIGH_IO_BASE         (0xFFFFFFFFFC000000)
#define KERN_HIGH_IO_SIZE         (KERN_HIGH_MEMORY_BASE - KERN_HIGH_IO_AREA)
                                     
#define KERN_MAX_HIGH_IO_PAGES    (KERN_HIGH_IO_SIZE / KERN_STRAIGHT_PAGE_SIZE)

#define PHY_TO_KERN_STRAIGHT(phy) ( ((uintptr_t)(phy)) + KERN_VMA_BASE)
#define KERN_STRAIGHT_TO_PHY(va)  ( ((uintptr_t)(va)) - KERN_VMA_BASE)
#define PHY_TO_PGFRAME_KVADDR(phy) ( ((uintptr_t)(phy)) + KERN_VMA_BASE)
#define HIGH_IO_TO_KERN_STRAIGHT(iaddr) \
	( ((uintptr_t)(iaddr)) + KERN_HIGH_IO_BASE - KERN_HIGH_IO_AREA)
#define KERN_STRAIGHT_TO_HIGH_IO(va)  \
	( ((uintptr_t)(va)) - KERN_HIGH_IO_BASE + KERN_HIGH_IO_AREA)

#if !defined(ASM_FILE)

#include <kern/freestanding.h>

typedef struct _kmap_info{
	void *phy_pml4;
	void *vir_pml4;
}kmap_info;

#define PAGE_ALIGNED(addr)						\
	( ( (uintptr_t)( ( (uintptr_t)(addr) ) &			\
		( (uintptr_t)(PAGE_SIZE) - (uintptr_t)1 ) ) ) == 0 )

#define PAGE_START(addr)					\
	( (uintptr_t)( ( (uintptr_t)(addr) ) &			\
	    ~( (uintptr_t)(PAGE_SIZE) - (uintptr_t)1 ) ) )

#define PAGE_NEXT(addr)				\
	( (uintptr_t)( PAGE_START( (addr) ) +	\
	(uintptr_t)PAGE_SIZE ) )

#define PAGE_END(addr)					\
	( (uintptr_t)( PAGE_NEXT( (addr) ) - 1 ) )

#define KERN_STRAIGHT_PAGE_ALIGNED(addr)						\
	( ( (uintptr_t)( ( (uintptr_t)(addr) ) &			\
		( (uintptr_t)(KERN_STRAIGHT_PAGE_SIZE) - (uintptr_t)1 ) ) ) == 0 )

#define KERN_STRAIGHT_PAGE_START(addr)					\
	( (uintptr_t)( ( (uintptr_t)(addr) ) &				\
	    ~( (uintptr_t)(KERN_STRAIGHT_PAGE_SIZE) - (uintptr_t)1 ) ) )

#define KERN_STRAIGHT_PAGE_NEXT(addr)			  \
	( (uintptr_t)KERN_STRAIGHT_PAGE_START( (addr) ) + \
	    (uintptr_t)(KERN_STRAIGHT_PAGE_SIZE) )

#define KERN_STRAIGHT_PAGE_END(addr)		\
	( KERN_STRAIGHT_PAGE_NEXT((addr)) - 1 )

void x64_map_kernel(size_t mem_max);
#endif  /*  ASM_FILE  */
#endif  /*  _HAL_KMAP_H   */
