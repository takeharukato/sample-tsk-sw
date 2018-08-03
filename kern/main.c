/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  main routine                                                      */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

/** カーネルの初期化
 */
void
kern_init(void) {
	
	idle_init_idle();

	sched_init();

	reaper_init_thread();

	irq_initialize_manager();

	buffer_cache_init();
	inode_cache_init();
	fdtable_init();
#if defined(CONFIG_HAL)
	hal_kernel_init();
#endif /*  CONFIG_HAL  */

	user_init();

	do_idle_loop();
}

#if !defined(CONFIG_HAL)
int 
main(int argc, char *argv[]) {

	init_pseudo_console();
	kheap_init();
	kern_init();
}
#endif  /*  !CONFIG_HAL  */
