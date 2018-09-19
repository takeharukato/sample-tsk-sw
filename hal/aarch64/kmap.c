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

#if defined(CONFIG_UPPERHALF_KERNEL)
static pgtbl_t  *upgtbl;
#endif  /*  CONFIG_UPPERHALF_KERNEL  */

/** map kernel area
 */
void
map_kernel_page(uintptr_t paddr, uintptr_t vaddr, uint64_t page_attr, void *kpgtbl) {
	lvl0_ent       ent0;
	lvl1_ent       ent1;
	lvl2_ent       ent2;
	level1_tbl *lvl1tbl;
	uintptr_t  lvl1addr;
	level2_tbl *lvl2tbl;
	uintptr_t  lvl2addr;
	void      *new_page;

        /*  Kernel virtual memory started from kernel 
	 * straight area, so we need to obtain 
	 * the offset address from the start address of
	 * kernel straight area. 
	 */
	vaddr = KERN_STRAIGHT_TO_PHY(vaddr);  

	/*
	 * Level 0 page table entry
	 */
	ent0 = get_level0_ent(kpgtbl, vaddr);
	if ( !AARCH64_PGTBL_PRESENT(ent0) ) {

		new_page = kheap_sbrk(PAGE_SIZE);
		kassert( new_page != HEAP_SBRK_FAILED );
		kassert( PAGE_ALIGNED(new_page) );

		lvl1tbl = (level1_tbl *)new_page;
		kassert((void *)lvl1tbl != NULL);
		memset((void *)&lvl1tbl->entries[0], 0, PAGE_SIZE);
		
		lvl1addr = (uintptr_t) KERN_STRAIGHT_TO_PHY(lvl1tbl);
		ent0 =  lvl1addr                             | 
			AARCH64_PGTBL_BLK_LOW_ATTR_AP_RW_EL0 |
			AARCH64_PGTBL_TBL_ENT;  
		set_level0_ent(kpgtbl, vaddr, ent0);
	} else {
		
		lvl1addr = get_level0_ent_addr(ent0);
	}
#if defined(DEBUG_SHOW_KERNEL_MAP)
	kprintf("boot_map_kernel_page LVL0->LVL1 (%p, %p):idx=%d lvl1addr=%p, ent=0x%lx\n",
	    paddr, vaddr, LEVEL0_INDEX(vaddr), lvl1addr, ent0);
#endif  /*  DEBUG_SHOW_KERNEL_MAP  */

	/*
	 * Level1 page table entry
	 */
	lvl1tbl = (level1_tbl *)PHY_TO_KERN_STRAIGHT(lvl1addr);
	ent1 = get_level1_ent(lvl1tbl, vaddr);
	if ( !AARCH64_PGTBL_PRESENT(ent1) ) {

		new_page = kheap_sbrk(PAGE_SIZE);
		kassert( new_page != HEAP_SBRK_FAILED );
		kassert( PAGE_ALIGNED(new_page) );

		lvl2tbl = (level2_tbl *)new_page;
		kassert((void *)lvl2tbl != NULL);
		memset((void *)&lvl2tbl->entries[0], 0, PAGE_SIZE);
		
		lvl2addr = (uintptr_t) KERN_STRAIGHT_TO_PHY(lvl2tbl);
		ent1 =  lvl2addr                               | 
			AARCH64_PGTBL_BLK_LOW_ATTR_AP_RW_EL0 |
			AARCH64_PGTBL_TBL_ENT;  
		set_level1_ent(lvl1tbl, vaddr, ent1);
	} else {
		
		lvl2addr = get_level1_ent_addr(ent1);
	}

#if defined(DEBUG_SHOW_KERNEL_MAP)
	kprintf("boot_map_kernel_page LVL1->LVL2 (%p, %p):idx=%d lvl2addr=%p, ent=0x%lx\n",
	    paddr, vaddr, LEVEL1_INDEX(vaddr), lvl2addr, ent1);
#endif  /*  DEBUG_SHOW_KERNEL_MAP  */

	/*
	 * Level2 page table entry
	 */
	lvl2tbl = (level2_tbl *)PHY_TO_KERN_STRAIGHT(lvl2addr);
	ent2 = get_level2_ent(lvl2tbl, vaddr);
	kassert( !AARCH64_PGTBL_PRESENT(ent2) );

	paddr = KERN_STRAIGHT_PAGE_START(paddr);
	page_attr &= AARCH64_PGTBL_BLK_ATTR_MASK;
	set_level2_ent(lvl2tbl, vaddr, 
	    paddr | page_attr | AARCH64_PGTBL_BLK_ENT);

#if defined(DEBUG_SHOW_KERNEL_MAP)
	kprintf("boot_map_kernel_page LVL2->PAGE (%p, %p):idx=%d paddr=%p, ent=0x%lx\n",
	    paddr, vaddr, LEVEL2_INDEX(vaddr), paddr, 
	    get_level2_ent(lvl2tbl, vaddr));
#endif  /*  DEBUG_SHOW_KERNEL_MAP  */
}

/** Setup AArch64 MMU
 */
void
aarch64_setup_mmu(void) {
	uint64_t      attr;
	uint64_t       reg;
	uintptr_t    paddr;

	/*
	 * Level 0 page table for kernel
	 */
	kpgtbl = kheap_sbrk(PAGE_SIZE);
	kassert( kpgtbl != HEAP_SBRK_FAILED );
	kassert( PAGE_ALIGNED(kpgtbl) );
	memset( &kpgtbl->entries[0], 0, PAGE_SIZE);

#if defined(CONFIG_UPPERHALF_KERNEL)
	/*
	 * Level 0 page table for user
	 */
	upgtbl = kheap_sbrk(PAGE_SIZE);
	kassert( upgtbl != HEAP_SBRK_FAILED );
	kassert( PAGE_ALIGNED(upgtbl) );
	memset( &upgtbl->entries[0], 0, PAGE_SIZE);
#endif  /*  CONFIG_UPPERHALF_KERNEL  */

	/*
	 * Making page table for MMIO devices
	 */
	for ( paddr = QEMU_VIRT_FLASH_BASE; 
	      ( QEMU_VIRT_FLASH_BASE + QEMU_VIRT_FLASH_SIZE ) > paddr;
	      paddr += KERN_STRAIGHT_PAGE_SIZE) { /* Flash device */

		map_kernel_page(paddr, PHY_TO_KERN_STRAIGHT(paddr), 
		    AARCH64_PGTBL_MMIO_ATTR, (void *)kpgtbl);
	}

	for ( paddr = QEMU_VIRT_CPUPERIPHS_BASE; ( QEMU_VIRT_MEM_BASE - 1 ) > paddr;
	      paddr += KERN_STRAIGHT_PAGE_SIZE) { /* CPU Peripherals */

		map_kernel_page(paddr, PHY_TO_KERN_STRAIGHT(paddr), 
		    AARCH64_PGTBL_MMIO_ATTR, (void *)kpgtbl);
	}

	/*
	 * Making page table for physical memory
	 */
	for (paddr = QEMU_VIRT_MEM_BASE; 
	     (QEMU_VIRT_MEM_BASE + (CONFIG_HAL_MEMORY_SIZE_MB * 1024 * 1024ULL) ) > paddr; 
	     paddr += KERN_STRAIGHT_PAGE_SIZE) { 

		map_kernel_page(paddr, PHY_TO_KERN_STRAIGHT(paddr), 
		    AARCH64_PGTBL_MEM_ATTR, (void *)kpgtbl);
	}

	/*
	 * Load page tables
	 */
#if defined(CONFIG_UPPERHALF_KERNEL)
	load_kpgtbl((uintptr_t)KERN_STRAIGHT_TO_PHY(kpgtbl));
	load_pgtbl((uintptr_t)KERN_STRAIGHT_TO_PHY(upgtbl));
#else
	load_pgtbl((uintptr_t)KERN_STRAIGHT_TO_PHY(kpgtbl));
#endif  /*  CONFIG_UPPERHALF_KERNEL  */

	/*
	 * Setup memory attributes
	 */
	attr= AARCH64_MAIR_VAL;
#if defined(DEBUG_SHOW_KERNEL_MAP)
	kprintf("MAIR_VAL: 0x%lx\n", AARCH64_MAIR_VAL);
#endif  /*  DEBUG_SHOW_KERNEL_MAP  */
	set_memory_attribute(attr);

	/*
	 * Setup Translation Control Register
	 */
	reg = AARCH64_TCR_VAL;
#if defined(DEBUG_SHOW_KERNEL_MAP)
	kprintf("TCR_VAL: 0x%lx\n", AARCH64_TCR_VAL);
#endif  /*  DEBUG_SHOW_KERNEL_MAP  */
	set_translation_control_register(reg);

	/*
	 * Enable MMU
	 */
	reg = get_system_control_register();
	reg |=  AARCH64_SCTLR_VAL;
#if defined(DEBUG_SHOW_KERNEL_MAP)
	kprintf("SCTLR_VAL: 0x%lx reg: 0x%lx\n", AARCH64_SCTLR_VAL, reg);
#endif  /*  DEBUG_SHOW_KERNEL_MAP  */
	set_system_control_register(reg);

	/*
	 * Synchronize context and ensure that no
	 * instructions are fetched using the old
	 * translation
	 */
	instruction_sync_barrier();
}
