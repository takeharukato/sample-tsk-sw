/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  kernel map                                                        */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

#include <hal/pgtbl.h>
#include <hal/board.h>
#include <hal/kmap.h>

//#define DEBUG_SHOW_KERNEL_MAP

static pgtbl_t  *kpgtbl;

/** map kernel area
 */
void
map_kernel_page(uintptr_t paddr, uintptr_t vaddr, uint64_t page_attr, void *kpgtbl) {
	int             rc;
	lvl1_ent       ent1;
	lvl1_ent       ent2;
	level2_tbl *lvl2tbl;
	uintptr_t  lvl2addr;
	void      *new_page;

	ent1 = get_level1_ent(kpgtbl, vaddr);
	if ( !AARCH64_PGTBL_PRESENT(ent1) ) {

		new_page = kheap_sbrk(PAGE_SIZE);
		kassert( new_page != HEAP_SBRK_FAILED );
		kassert( PAGE_ALIGNED(new_page) );

		lvl2tbl = (level2_tbl *)new_page;
		kassert((void *)lvl2tbl != NULL);
		memset((void *)lvl2tbl, 0, PAGE_SIZE);
		
		lvl2addr = (uintptr_t) KERN_STRAIGHT_TO_PHY(lvl2tbl);
		ent1 =  lvl2addr                               | 
			AARCH64_PGTBL12_BLK_LOW_ATTR_AP_RW_EL0 |
			AARCH64_PGTBL_TBL_ENT;  
		set_level1_ent(kpgtbl, vaddr, ent1);
	} else {
		
		lvl2addr = get_level1_ent_addr(ent1);
	}

#if defined(DEBUG_SHOW_KERNEL_MAP)
	kprintf("boot_map_kernel_page LVL1->LVL2 (%p, %p):lvl2addr=%p, ent=0x%lx\n",
	    paddr, vaddr, lvl2addr, ent1);
#endif  /*  DEBUG_SHOW_KERNEL_MAP  */

	lvl2addr = PHY_TO_KERN_STRAIGHT(lvl2addr);
	ent2 = get_level2_ent((level2_tbl *)lvl2addr, vaddr);
	kassert( !AARCH64_PGTBL_PRESENT(ent2) );

	paddr = KERN_STRAIGHT_PAGE_START(paddr);
	page_attr &= ( ( AARCH64_PGTBL12_BLK_LOW_BLK_ATTR_MASK << 
	    AARCH64_PGTBL12_BLK_LOW_BLK_ATTR_SHIFT ) |
		( AARCH64_PGTBL12_BLK_UPPER_BLK_ATTR_MASK << 
		    AARCH64_PGTBL12_BLK_UPPER_BLK_ATTR_SHIFT ) );
	set_level2_ent((level2_tbl *)lvl2addr, vaddr, 
	    paddr | page_attr | AARCH64_PGTBL_BLK_ENT);
#if defined(DEBUG_SHOW_KERNEL_MAP)
	kprintf("boot_map_kernel_page LVL2->PAGE (%p, %p):paddr=%p, ent=0x%lx\n",
	    paddr, vaddr, paddr, get_level2_ent((level2_tbl *)lvl2addr, vaddr));
#endif  /*  DEBUG_SHOW_KERNEL_MAP  */
}

/** Setup AArch64 MMU
 */
void
setup_mmu(void) {
	uint64_t      attr;
	uint64_t page_attr;
	uint64_t       reg;
	uintptr_t    paddr;
	
	kpgtbl = kheap_sbrk(PAGE_SIZE);
	kassert( kpgtbl != HEAP_SBRK_FAILED );
	kassert( PAGE_ALIGNED(kpgtbl) );

	memset( kpgtbl, 0, PAGE_SIZE);

	attr= (  ( (AARCH64_MAIR_ATTR_UPPER_DEVMEM | AARCH64_MAIR_ATTR_LOWER_DEV_NG_NR_NE)
		   << AARCH64_MAIR_ATTR0_SHIFT ) |
		 ( (AARCH64_MAIR_ATTR_UPPER_DEVMEM | AARCH64_MAIR_ATTR_LOWER_DEV_NG_NR_E) 
		   << AARCH64_MAIR_ATTR1_SHIFT ) |
		 ( (AARCH64_MAIR_ATTR_UPPER_MEM_ONC | AARCH64_MAIR_ATTR_LOWER_MEM_INC)
		   << AARCH64_MAIR_ATTR2_SHIFT ) |
		 ( (AARCH64_MAIR_ATTR_UPPER_MEM_OWBNT | AARCH64_MAIR_ATTR_LOWER_MEM_IWBNTRW )
		   << AARCH64_MAIR_ATTR3_SHIFT ) );

	set_memory_attribute(attr);

	for ( paddr = QEMU_VIRT_FLASH_BASE; 
	      ( QEMU_VIRT_FLASH_BASE + QEMU_VIRT_FLASH_SIZE ) > paddr;
	      paddr += KERN_STRAIGHT_PAGE_SIZE) {

		page_attr = AARCH64_PGTBL12_BLK_LOW_ATTR_AF_ENABLE | 
			AARCH64_PGTBL12_BLK_LOW_ATTR_SH_IS| 
			0 << AARCH64_PGTBL12_BLK_LOW_ATTR_IDX_SHIFT;
		map_kernel_page(paddr, PHY_TO_KERN_STRAIGHT(paddr), 
		    page_attr, (void *)kpgtbl);
	}

	for ( paddr = QEMU_VIRT_CPUPERIPHS_BASE; ( QEMU_VIRT_MEM_BASE - 1 ) > paddr;
	      paddr += KERN_STRAIGHT_PAGE_SIZE) {

		page_attr = AARCH64_PGTBL12_BLK_LOW_ATTR_AF_ENABLE | 
			AARCH64_PGTBL12_BLK_LOW_ATTR_SH_IS| 
			0 << AARCH64_PGTBL12_BLK_LOW_ATTR_IDX_SHIFT;
		map_kernel_page(paddr, PHY_TO_KERN_STRAIGHT(paddr), 
		    page_attr, (void *)kpgtbl);
	}
	
	for (paddr = QEMU_VIRT_MEM_BASE; 
	     (QEMU_VIRT_MEM_BASE + (CONFIG_HAL_MEMORY_SIZE_MB * 1024 * 1024ULL) ) > paddr; 
	     paddr += KERN_STRAIGHT_PAGE_SIZE) { 

		page_attr = AARCH64_PGTBL12_BLK_LOW_ATTR_AF_ENABLE | 
			AARCH64_PGTBL12_BLK_LOW_ATTR_SH_IS| 
			3 << AARCH64_PGTBL12_BLK_LOW_ATTR_IDX_SHIFT;
		map_kernel_page(paddr, PHY_TO_KERN_STRAIGHT(paddr), 
		    page_attr, (void *)kpgtbl);
	}

	load_pgtbl((uintptr_t)kpgtbl);

	reg = (   ( (uint64_t)(0x0)) << AARCH64_TCR_EL1_TBI0_SHIFT)    | // TBI=0, The size offset = 0
		( ((uint64_t)(AARCH64_TCR_IPS_4GB)) << AARCH64_TCR_EL1_IPS_SHIFT)| // IPS= 32 bit
		( ((uint64_t)AARCH64_TCR_TG1_4KB) << AARCH64_TCR_EL1_TG1_SHIFT) | // TG1=4k
		( ((uint64_t)AARCH64_TCR_SH_INNER_SHARE) << AARCH64_TCR_EL1_SH1_SHIFT) | // SH1=3 inner 
		( ((uint64_t)AARCH64_TCR_ORGN_WBRAWA) << AARCH64_TCR_EL1_ORGN1_SHIFT) | // ORGN1=1 
		( ((uint64_t)AARCH64_TCR_IRGN_WBRAWA) << AARCH64_TCR_EL1_IRGN1_SHIFT) | // IRGN1=1 
		( ((uint64_t)AARCH64_TCR_EPD_DISABLE)  << AARCH64_TCR_EL1_EPD1_SHIFT) | // EPD1
		( ((uint64_t)AARCH64_TCR_A1_TTBR0) << AARCH64_TCR_EL1_A1_SHIFT)       | // A1=0
		(((uint64_t)AARCH64_TCR_TSZ_4G)   << AARCH64_TCR_EL1_T1SZ_SHIFT) | // T1SZ=32 (4G)
		(((uint64_t)AARCH64_TCR_TG0_4KB) << AARCH64_TCR_EL1_TG0_SHIFT) | // TG0=4k
		(((uint64_t)AARCH64_TCR_SH_INNER_SHARE ) << AARCH64_TCR_EL1_SH0_SHIFT) | // SH0=3
		(((uint64_t)AARCH64_TCR_ORGN_WBRAWA) << AARCH64_TCR_EL1_ORGN0_SHIFT) | // ORGN0=1
		(((uint64_t)AARCH64_TCR_IRGN_WBRAWA) << AARCH64_TCR_EL1_IRGN0_SHIFT)  | // IRGN0=1
		(((uint64_t)AARCH64_TCR_EPD_ENABLE)  << AARCH64_TCR_EL1_EPD0_SHIFT)  | //  EPD0=0 
		(((uint64_t)AARCH64_TCR_TSZ_4G)   << 0);   // T0SZ=32 (4G)

	set_translation_control_register(reg);

	reg = get_system_control_register();
	reg |=    AARCH64_SCTLR_RESVD_BITS |   /* Mandatory reserved bits */
	        AARCH64_SCTLR_I          |   /* Instruction cache enable  */
		AARCH64_SCTLR_SA0        |   /* Stack Alignment Check Enable for EL0 */
		AARCH64_SCTLR_SA         |   /* Stack Alignment Check Enable */
		AARCH64_SCTLR_C          |   /* Data cache enable */
		AARCH64_SCTLR_A          |   /* Alignment check enable */
		AARCH64_SCTLR_M;             /* Enable MMU */
	set_system_control_register(reg);
	instruction_sync_barrier();
}
