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
#include <kern/heap.h>

#include <hal/hal.h>
#include <hal/uart.h>
#include <hal/kmap.h>

void
hal_kernel_init(void){
}

/** 64bit モードでのブートアップ
 */
void 
boot_main(uint64_t __attribute__ ((unused)) magic, 
    uint64_t __attribute__ ((unused)) mbaddr) {

	uart8250_init();

	kprintf("OS sample for X64 booted.\n");
	x64_map_kernel(CONFIG_HAL_MEMORY_SIZE_MB*1024UL*1024);
	kprintf("kernel mapped \n");
	while(1);
}
