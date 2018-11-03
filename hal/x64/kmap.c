/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2018 Takeharu KATO                                      */
/*                                                                    */
/*  Kernel memory map                                                 */
/*                                                                    */
/**********************************************************************/

#include <kern/freestanding.h>
#include <kern/printf.h>
#include <kern/string.h>
#include <kern/heap.h>

#include <hal/pgtbl.h>

//#define DEBUG_SHOW_KERNEL_MAP

extern void *kmap_pml4;

static kmap_info kmap_pgtbl={&kmap_pml4, (void *)PHY_TO_KERN_STRAIGHT(&kmap_pml4)};

static void 
map_high_io_area(void *kpgtbl) {
	uintptr_t  cur_page;
	uintptr_t cur_vaddr;
	uintptr_t last_addr;	

	/*
	 * High I/O memory
	 */
	kprintf("High APIC I/O region : [%p, %p] to %p\n",
	    (void *)KERN_HIGH_IO_AREA,
	    (void *)( KERN_HIGH_IO_AREA + KERN_HIGH_IO_SIZE ),
	    (void *)KERN_HIGH_IO_BASE );

	last_addr = KERN_STRAIGHT_PAGE_END( KERN_HIGH_IO_AREA + 
	    KERN_HIGH_IO_SIZE );

	for( cur_page = KERN_STRAIGHT_PAGE_START( KERN_HIGH_IO_AREA ), 
		     cur_vaddr = KERN_HIGH_IO_BASE;
	     cur_page <= last_addr;
	     cur_page += KERN_STRAIGHT_PAGE_SIZE, 
		     cur_vaddr += KERN_STRAIGHT_PAGE_SIZE) {

		map_kernel_page( cur_page,  cur_vaddr, PAGE_WRITABLE | PAGE_NONCACHABLE, 
		    kpgtbl);
	}
}

void
map_kernel_page(uintptr_t paddr, uintptr_t vaddr, 
    uint64_t page_attr, void *kpgtbl) {
	pml4e          ent1;
	pdpe           ent2;
	pdire          ent3;
	uintptr_t  pdp_addr;
	uintptr_t pdir_addr;
	void      *new_page;

	ent1 = get_pml4_ent((pgtbl_t *)kpgtbl, vaddr);
	if ( !pdp_present(ent1) ) {

		new_page = kheap_sbrk(PAGE_SIZE);
		kassert( new_page != HEAP_SBRK_FAILED );

		pdp_addr = (uintptr_t)new_page;
		kassert((void *)pdp_addr != NULL);
		memset((void *)pdp_addr, 0, PAGE_SIZE);

		pdp_addr = KERN_STRAIGHT_TO_PHY(pdp_addr);
		set_pml4_ent((pgtbl_t *)kpgtbl, vaddr, pdp_addr | PAGE_PRESENT);
		ent1 = get_pml4_ent((pgtbl_t *)kpgtbl, vaddr);
	} else {
		
		pdp_addr = get_ent_addr(ent1);
	}

#if defined(DEBUG_SHOW_KERNEL_MAP)
	kprintf("boot_map_kernel_page PML4->PDP (%p, %p):pdp=%p, ent=0x%lx\n",
	    paddr, vaddr, pdp_addr, ent1);
#endif  /*  DEBUG_SHOW_KERNEL_MAP  */

	pdp_addr = PHY_TO_KERN_STRAIGHT(pdp_addr);
	ent2 = get_pdp_ent((pdp_tbl *)pdp_addr, vaddr);
	if (!pdir_present(ent2)) {

		new_page = kheap_sbrk(PAGE_SIZE);
		kassert( new_page != HEAP_SBRK_FAILED );

		pdir_addr = (uintptr_t)new_page;
		kassert((void *)pdir_addr != NULL);
		memset((void *)pdir_addr, 0, PAGE_SIZE);

		pdir_addr = KERN_STRAIGHT_TO_PHY(pdir_addr);
		set_pdp_ent((pdp_tbl *)pdp_addr, vaddr, pdir_addr | PAGE_PRESENT);
		ent2 = get_pdp_ent((pdp_tbl *)pdp_addr, vaddr);
	} else {
		
		pdir_addr = get_ent_addr(ent2);
	}

#if defined(DEBUG_SHOW_KERNEL_MAP)
	kprintf("boot_map_kernel_page PDP->PDIR (%p, %p):pdir=%p, ent=0x%lx\n",
	    paddr, vaddr, pdir_addr, ent2);
#endif  /*  DEBUG_SHOW_KERNEL_MAP  */

	pdir_addr = PHY_TO_KERN_STRAIGHT(pdir_addr);
	ent3 = get_pdir_ent((pdir_tbl *)pdir_addr, vaddr);
	kassert(!pdir_present(ent3));
	paddr = KERN_STRAIGHT_PAGE_START(paddr);
	page_attr &= (PAGE_SIZE - 1);
	set_pdir_ent((pdir_tbl *)pdir_addr, vaddr, 
	    paddr | page_attr | PAGE_PRESENT | PAGE_2MB );
#if defined(DEBUG_SHOW_KERNEL_MAP)
	kprintf("boot_map_kernel_page PDIR->PAGE (%p, %p):paddr=%p, ent=0x%lx\n",
	    paddr, vaddr, paddr, get_pdir_ent((pdir_tbl *)pdir_addr, vaddr));
#endif  /*  DEBUG_SHOW_KERNEL_MAP  */
}

void
x64_map_kernel(size_t mem_max) {
	uintptr_t  phy_last_mem;
	uintptr_t      cur_page;
	uintptr_t     cur_vaddr;
	uintptr_t    start_addr;
	uintptr_t     last_addr;

	memset((void *)kmap_pgtbl.phy_pml4, 0, PAGE_SIZE);

	start_addr = KERN_STRAIGHT_PAGE_START(KERN_PHY_MIN);
	phy_last_mem = ( ( KERN_PHY_MAX >  mem_max ) ? ( mem_max ) : KERN_PHY_MAX ) - 1;
	last_addr = KERN_STRAIGHT_PAGE_END( phy_last_mem );

	/*
	 * kernel straight map
	 */
	kprintf("boot-map-kernel : [%p, %p] to %p\n",
	    (void *)start_addr, (void *)last_addr, KERN_VMA_BASE);


	for( cur_page = start_addr, cur_vaddr = KERN_VMA_BASE;
	     cur_page <= last_addr;
	     cur_page += KERN_STRAIGHT_PAGE_SIZE, cur_vaddr += KERN_STRAIGHT_PAGE_SIZE) {

		map_kernel_page( cur_page,  cur_vaddr, PAGE_WRITABLE, kmap_pgtbl.phy_pml4 );
	}
	map_high_io_area(kmap_pgtbl.phy_pml4);
	load_pgtbl((uintptr_t)kmap_pgtbl.phy_pml4);
}
