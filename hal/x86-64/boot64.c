/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  64bit boot code                                                   */
/*                                                                    */
/**********************************************************************/

#include <kern/freestanding.h>
#include <kern/printf.h>

#include <hal/hal.h>

#define CHECK_FLAG(flags,bit)   \
	((flags) & (1 << (bit))) /* Check if the bit BIT in FLAGS is set. */

/** ブートヘッダを表示
 */
static void
show_boot_headers(multiboot_info_t *mbi) {
	
	/* Clear the screen. */
	kcls ();

	/* Print out the flags. */
	kprintf ("mbi = %x flags = 0x%x\n",(unsigned long)mbi, (unsigned long) mbi->flags);
     
	/* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0))
		kprintf ("mem_lower = %uKB, mem_upper = %uKB\n",
		    (unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);
     
	/* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		kprintf ("boot_device = 0x%x\n", (unsigned) mbi->boot_device);
     
	/* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
	  kprintf ("cmdline = %s\n", (char *)(uint64_t)mbi->cmdline);
     
	/* Are mods_* valid? */
	if (CHECK_FLAG (mbi->flags, 3)) {
		    multiboot_module_t *mod;
		    int i;
     
		    kprintf ("mods_count = %d, mods_addr = 0x%x\n",
			(int) mbi->mods_count, (int) mbi->mods_addr);
		    for (i = 0, mod = (multiboot_module_t *)(uint64_t)mbi->mods_addr;
			 i < mbi->mods_count;
			 i++, mod++)
			    kprintf (" mod_start = 0x%x, mod_end = 0x%x, cmdline = %s\n",
				(unsigned) mod->mod_start,
				(unsigned) mod->mod_end,
				    (char *)(uint64_t) mod->cmdline);
	    }
	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6)) {
		    multiboot_memory_map_t *mmap;
     
		    kprintf ("mmap_addr = 0x%x, mmap_length = 0x%x\n",
			(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
		    for (mmap = (multiboot_memory_map_t *)(uint64_t)mbi->mmap_addr;
			 (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
			 mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
			     + mmap->size + sizeof (mmap->size)))
			    kprintf (" size = 0x%x, base_addr = 0x%x%x,"
				" length = 0x%x%x, type = 0x%x\n",
				(unsigned) mmap->size,
				mmap->addr >> 32,
				mmap->addr & 0xffffffff,
				mmap->len >> 32,
				mmap->len & 0xffffffff,
				(unsigned) mmap->type);
	    }


	return ;
}

void
hal_kernel_init(void){
}

/** 64bit モードでのブートアップ
 */
void 
boot_main(unsigned long magic, unsigned long addr) {

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		    kprintf ("Invalid magic number: 0x%x\n", (unsigned) magic);
		    panic("Can not boot kernel\n");
	}

	init_raw_console();
	show_boot_headers((multiboot_info_t *)addr);

	init_descriptor();

	kern_init();

	while(1);
}
