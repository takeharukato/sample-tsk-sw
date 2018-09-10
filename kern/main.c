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
	
	/*
	 * File system should be initialized before thread creation (including kernel thread)
	 * because thr_thread_start() opens stdin, stdout, stderr.
	 */
#if defined(CONFIG_HAL)
	devsw_init();
	buffer_cache_init();
	inode_cache_init();
	fdtable_init();
#endif  /*  CONFIG_HAL  */

	idle_init_idle();
	sched_init();
	reaper_init_thread();

#if defined(CONFIG_HAL)
	irq_initialize_manager();

	callout_init();

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

	return 0;
}
#endif  /*  !CONFIG_HAL  */
