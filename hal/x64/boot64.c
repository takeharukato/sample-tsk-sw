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
#include <hal/thread_info.h>
#include <hal/uart.h>
#include <hal/kmap.h>
#include <hal/segments.h>
#include <hal/i8254.h>

extern char _fsimg_start;
extern char _fsimg_end;

static struct _x64_cpu{
	void           *gdtp;
	idt_descriptor *idtp;
	tss64          *tssp;
}x64_cpu;

void
hal_kernel_init(void){
	thread_t *idle;
	thread_attr_t *attrp;

	idle = idle_refer_idle_thread();
	attrp = &idle->attr;

	attrp->stack_top = &bsp_stack;
	attrp->stack_size = STACK_SIZE;

	idle->tinfo = attrp->stack_top + idle->attr.stack_size - sizeof(thread_info_t);
	idle->tinfo->thr = idle;

	x64_init_pic();
	x64_timer_init();
}

/** 64bit モードでのブートアップ
 */
void 
boot_main(uint64_t __attribute__ ((unused)) magic, 
    uint64_t __attribute__ ((unused)) mbaddr) {
	char c;

	uart8250_init();

	c = *(char *)&_fsimg_start;

	kprintf("OS sample for X64 booted.\n");

	x64_map_kernel(CONFIG_HAL_MEMORY_SIZE_MB*1024UL*1024);

	x64_cpu.gdtp = kheap_sbrk(PAGE_SIZE);
	kassert( x64_cpu.gdtp != HEAP_SBRK_FAILED );
	x64_init_segments(x64_cpu.gdtp, (tss64 **)(&x64_cpu.tssp));
	x64_init_idt(&x64_cpu.idtp);

	kern_init();

	while(1);
}
