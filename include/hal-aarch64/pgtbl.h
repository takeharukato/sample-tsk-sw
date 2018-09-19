/* -*- mode: C; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  AArch64 page table definitions                                    */
/*                                                                    */
/**********************************************************************/
#if !defined(__HAL_PGTBL_H)
#define __HAL_PGTBL_H

#include <kern/param.h>
/*
 * see D4.2.4 Translation tables and the translation process in
 * ARM Architecture Reference Manual for ARMv8-A
 */
#define LEVEL0_INDEX_SHIFT       (39)
#define LEVEL1_INDEX_SHIFT       (30)
#define LEVEL2_INDEX_SHIFT       (21)
#define LEVEL3_INDEX_SHIFT       (12)

#define LEVEL0_INDEX_MASK       (0x1ff)
#define LEVEL1_INDEX_MASK       (0x1ff)
#define LEVEL2_INDEX_MASK       (0x1ff)
#define LEVEL3_INDEX_MASK       (0x1ff)

#define LEVEL0_INDEX(vaddr)     ( ( (vaddr) >> LEVEL0_INDEX_SHIFT   ) & LEVEL0_INDEX_MASK)
#define LEVEL1_INDEX(vaddr)     ( ( (vaddr) >> LEVEL1_INDEX_SHIFT   ) & LEVEL1_INDEX_MASK)
#define LEVEL2_INDEX(vaddr)     ( ( (vaddr) >> LEVEL2_INDEX_SHIFT   ) & LEVEL2_INDEX_MASK)
#define LEVEL3_INDEX(vaddr)     ( ( (vaddr) >> LEVEL3_INDEX_SHIFT   ) & LEVEL3_INDEX_MASK)

#define PGTBL_ENTRY_SIZE        (8)
#define PGTBL_ENTRY_MAX         ( PAGE_SIZE / (PGTBL_ENTRY_SIZE) )

#define AARCH64_PGTBL_PG2PFN_4KB(x)  ( (x) >> LEVEL3_INDEX_SHIFT )
#define AARCH64_PGTBL_PG2PFN_2MIB(x) ( (x) >> LEVEL2_INDEX_SHIFT )

/** 
 * See D4.3.1 VMSAv8-64 translation table level 0, level 1, and level 2 descriptor formats in
 * ARM Architecture Reference Manual ARMv8, for ARMv8-A architecture profile
 */
#define AARCH64_PGTBL_INVALID                     (ULL_C(0))
#define AARCH64_PGTBL_BLK_ENT                     (ULL_C(0x1))
#define AARCH64_PGTBL_TBL_ENT                     (ULL_C(0x3))
#define AARCH64_PGTBL_TYPE_MASK                   (ULL_C(0x3))
#define AARCH64_PGTBL_PRESENT(ent)                ( (ent) & AARCH64_PGTBL_TYPE_MASK )

#define AARCH64_PGTBL1_BLK_OUTADDR_4K_SHIFT       (30)
#define AARCH64_PGTBL2_BLK_OUTADDR_4K_SHIFT       (21)
#define AARCH64_PGTBL2_BLK_OUTADDR_16K_SHIFT      (25)
#define AARCH64_PGTBL2_BLK_OUTADDR_64K_SHIFT      (29)

#define AARCH64_PGTBL12_TBL_NEXT_PGTBL_4K_SHIFT   (12)
#define AARCH64_PGTBL12_TBL_NEXT_PGTBL_16K_SHIFT  (14)
#define AARCH64_PGTBL12_TBL_NEXT_PGTBL_64K_SHIFT  (16)
#define AARCH64_PGTBL12_TBL_PXNTBL_SHIFT          (59)
#define AARCH64_PGTBL12_TBL_XNTBL_SHIFT           (60)
#define AARCH64_PGTBL12_TBL_NSTBL_SHIFT           (63)
#define AARCH64_PGTBL12_TBL_NEXT_PGTBL_4K_MASK    (ULL_C(0xfffffffff)) 
#define AARCH64_PGTBL12_TBL_NEXT_PGTBL_16K_MASK   (ULL_C(0x3ffffffff))
#define AARCH64_PGTBL12_TBL_NEXT_PGTBL_64K_MASK   (ULL_C(0x0ffffffff))
#define AARCH64_PGTBL12_TBL_PXNTBL                (ULL_C(0x0800000000000000))
#define AARCH64_PGTBL12_TBL_XNTBL                 (ULL_C(0x1000000000000000))
#define AARCH64_PGTBL12_TBL_APTBL_SHIFT           (61)
#define AARCH64_PGTBL12_TBL_NSTBL                 (ULL_C(0x8000000000000000))

#define AARCH64_PGTBL12_BLK_INVALID               (0)
#define AARCH64_PGTBL12_BLK_BLOCK                 (1)
#define AARCH64_PGTBL12_BLK_NEXT_TABLE            (3)

#define AARCH64_PGTBL3_BLK_OUTADDR_4K_SHIFT       (12)
#define AARCH64_PGTBL3_BLK_OUTADDR_16K_SHIFT      (14)
#define AARCH64_PGTBL3_BLK_OUTADDR_64K_SHIFT      (16)
#define AARCH64_PGTBL3_BLK_64K_TA_SHIFT           (12)
#define AARCH64_PGTBL3_BLK_64K_TA_MASK            (ULL_C(0xf))

#define AARCH64_PGTBL3_BLK_UPPER_ATTR_DBM_SHIFT          (51)
#define AARCH64_PGTBL3_BLK_UPPER_ATTR_CONTIGUOUS_SHIFT   (52)
#define AARCH64_PGTBL3_BLK_UPPER_ATTR_PXN_SHIFT          (53)
#define AARCH64_PGTBL3_BLK_UPPER_ATTR_UXN_SHIFT          (54)
#define AARCH64_PGTBL3_BLK_UPPER_ATTR_XN_SHIFT           (54)
#define AARCH64_PGTBL3_BLK_UPPER_ATTR_PBHA_SHIFT         (59)

#define AARCH64_PGTBL3_BLK_UPPER_ATTR_DBM         ( ULL_C(0x08000000000000) )
#define AARCH64_PGTBL3_BLK_UPPER_ATTR_CONTIGUOUS  ( ULL_C(0x10000000000000) )
#define AARCH64_PGTBL3_BLK_UPPER_ATTR_PXN         ( ULL_C(0x20000000000000) )
#define AARCH64_PGTBL3_BLK_UPPER_ATTR_UXN         ( ULL_C(0x40000000000000) )
#define AARCH64_PGTBL3_BLK_UPPER_ATTR_UXN         ( ULL_C(0x40000000000000) )

#define AARCH64_PGTBL_BLK_LOW_BLK_ATTR_SHIFT     (2)
#define AARCH64_PGTBL_BLK_LOW_BLK_ATTR_MASK      (ULL_C(0x3ff))

#define AARCH64_PGTBL_BLK_LOW_ATTR_TBL_SHIFT     (0)
#define AARCH64_PGTBL_BLK_LOW_ATTR_IDX_SHIFT     (2)

#define AARCH64_PGTBL_BLK_LOW_ATTR_NS_SHIFT      (5)
#define AARCH64_PGTBL_BLK_LOW_ATTR_NS_NOSECURE   \
	(ULL_C(0x1) << AARCH64_PGTBL12_BLK_LOW_ATTR_NS_SHIFT)
#define AARCH64_PGTBL_BLK_LOW_ATTR_NS_SECURE   \
	(ULL_C(0x0) << AARCH64_PGTBL12_BLK_LOW_ATTR_NS_SHIFT)

#define AARCH64_PGTBL_BLK_LOW_ATTR_AP_SHIFT      (6)
#define AARCH64_PGTBL_BLK_LOW_ATTR_AP_RW_ELHIGH  ( ULL_C(0x0) )
#define AARCH64_PGTBL_BLK_LOW_ATTR_AP_RW_EL0     ( ULL_C(0x40) )
#define AARCH64_PGTBL_BLK_LOW_ATTR_AP_RO_ELHIGH  ( ULL_C(0x80) )
#define AARCH64_PGTBL_BLK_LOW_ATTR_AP_RO_EL0     ( ULL_C(0xc0) )

#define AARCH64_PGTBL_BLK_LOW_ATTR_SH_SHIFT      (8)
#define AARCH64_PGTBL_BLK_LOW_ATTR_SH_NS         ( ULL_C(0)     )
#define AARCH64_PGTBL_BLK_LOW_ATTR_SH_OS         ( ULL_C(0x200) )
#define AARCH64_PGTBL_BLK_LOW_ATTR_SH_IS         ( ULL_C(0x300) )

#define AARCH64_PGTBL_BLK_LOW_ATTR_AF_SHIFT      (10)
#define AARCH64_PGTBL_BLK_LOW_ATTR_AF_ENABLE     (ULL_C(0x400))
#define AARCH64_PGTBL_BLK_LOW_ATTR_AF_DISABLE    (ULL_C(0x0)  )

#define AARCH64_PGTBL_BLK_LOW_ATTR_NG_SHIFT      (11)

#define AARCH64_PGTBL_BLK_UPPER_BLK_ATTR_SHIFT  (51)
#define AARCH64_PGTBL_BLK_UPPER_BLK_ATTR_MASK   (ULL_C(0xfff))

#define AARCH64_PGTBL_BLK_ATTR_MASK					\
	( ( AARCH64_PGTBL_BLK_LOW_BLK_ATTR_MASK << AARCH64_PGTBL_BLK_LOW_BLK_ATTR_SHIFT ) | \
	( AARCH64_PGTBL_BLK_UPPER_BLK_ATTR_MASK << AARCH64_PGTBL_BLK_UPPER_BLK_ATTR_SHIFT ) ) 

#define AARCH64_PGTBL_MMIO_ATTR						            \
	( AARCH64_PGTBL_BLK_LOW_ATTR_AF_ENABLE | AARCH64_PGTBL_BLK_LOW_ATTR_SH_IS | \
	    ( AARCH64_MAIR_DEV_nGnRnE_ATTR << AARCH64_PGTBL_BLK_LOW_ATTR_IDX_SHIFT ) )

#define AARCH64_PGTBL_MEM_ATTR						            \
	( AARCH64_PGTBL_BLK_LOW_ATTR_AF_ENABLE | AARCH64_PGTBL_BLK_LOW_ATTR_SH_IS | \
	    ( AARCH64_MAIR_MEM_ATTR << AARCH64_PGTBL_BLK_LOW_ATTR_IDX_SHIFT ) )

#if !defined(ASM_FILE)

#include <kern/freestanding.h>

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

typedef uintptr_t lvl0_ent;
typedef uintptr_t lvl1_ent;
typedef uintptr_t lvl2_ent;
typedef uintptr_t lvl3_ent;

typedef struct _level0_tbl {
	lvl0_ent entries[PGTBL_ENTRY_MAX];
}level0_tbl;

typedef struct _level1_tbl {
	lvl1_ent entries[PGTBL_ENTRY_MAX];
}level1_tbl;

typedef struct _level2_tbl {
	lvl2_ent entries[PGTBL_ENTRY_MAX];
}level2_tbl;

typedef struct _level3_tbl {
	lvl3_ent entries[PGTBL_ENTRY_MAX];
}level3_tbl;

typedef level1_tbl pgtbl_t;

static inline lvl0_ent
get_level0_ent(level0_tbl *lvl0tbl, uintptr_t vaddr) {

	return lvl0tbl->entries[LEVEL0_INDEX(vaddr)];
}

static inline lvl1_ent
get_level1_ent(level1_tbl *lvl1tbl, uintptr_t vaddr) {

	return lvl1tbl->entries[LEVEL1_INDEX(vaddr)];
}

static inline lvl2_ent
get_level2_ent(level2_tbl *lvl2tbl, uintptr_t vaddr) {

	return lvl2tbl->entries[LEVEL2_INDEX(vaddr)];
}

static inline lvl3_ent
get_level3_ent(level3_tbl *lvl3tbl, uintptr_t vaddr) {

	return lvl3tbl->entries[LEVEL3_INDEX(vaddr)];
}

static inline uintptr_t
get_level0_ent_addr(uintptr_t ent) {

	return ( PAGE_START(ent) & ~AARCH64_PGTBL_BLK_ATTR_MASK );
}

static inline uintptr_t
get_level1_ent_addr(uintptr_t ent) {

	return ( PAGE_START(ent) & ~AARCH64_PGTBL_BLK_ATTR_MASK );
}

static inline uintptr_t
get_level12_ent_attr(uintptr_t ent) {

	return  ( ent & AARCH64_PGTBL_BLK_ATTR_MASK );
}

static inline void
set_level0_ent(level0_tbl *lvl0, uintptr_t vaddr, uintptr_t val) {

	lvl0->entries[LEVEL0_INDEX(vaddr)] = val;
}

static inline void
set_level1_ent(level1_tbl *lvl1, uintptr_t vaddr, uintptr_t val) {

	lvl1->entries[LEVEL1_INDEX(vaddr)] = val;
}

static inline void
set_level2_ent(level2_tbl *lvl2, uintptr_t vaddr, uintptr_t val) {

	lvl2->entries[LEVEL2_INDEX(vaddr)] = val;
}

static inline void
set_level3_ent(level3_tbl *lvl3, uintptr_t vaddr, uintptr_t val) {

	lvl3->entries[LEVEL3_INDEX(vaddr)] = val;
}

static inline void
instruction_sync_barrier(void){

	__asm__ __volatile__ ("isb\n\t"::: "memory");
}

static inline void
data_sync_barrier(void){

	__asm__ __volatile__ ("dsb sy\n\t"::: "memory");
}

static inline void
set_memory_attribute(uint64_t reg) {

	__asm__ __volatile__ ("msr mair_el1, %0\n\t"::"r" (reg) : "memory");
}

static inline void
set_translation_control_register(uint64_t reg) {

	__asm__ __volatile__ ("msr tcr_el1, %0\n\t"::"r" (reg) : "memory");
}

static inline void
invalidate_tlb(void){
	
}

static inline void 
load_pgtbl(const uintptr_t pgtbl_addr) {

	__asm__ __volatile__ ("msr ttbr0_el1, %0" : : "r"(pgtbl_addr) : "memory");
	instruction_sync_barrier();

	return;
}

static inline void 
load_kpgtbl(const uintptr_t pgtbl_addr) {

	__asm__ __volatile__ ("msr ttbr1_el1, %0" : : "r"(pgtbl_addr) : "memory");
	instruction_sync_barrier();

	return;
}
#endif  /*  !ASM_FILE  */

#endif  /*  __HAL_PGTBL_H  */
