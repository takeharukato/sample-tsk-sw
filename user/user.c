/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  User tasks                                                        */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

void
threadA(void *arg) {

	while(1) {

		kprintf("threadA\n");

		sched_rotate_queue();
		sched_schedule();
	}
}

void
threadB(void *arg) {

	while(1) {

		kprintf("threadB\n");

		sched_rotate_queue();
		sched_schedule();
	}
}
void
threadC(void *arg) {
	int fd;
	char data[64];

	kprintf("threadC\n");

	fd = sys_open("/test.txt", O_RDWR|O_CREATE);
	if ( fd >= 0 ) {
		sys_write(fd, "hello world", strlen("hello world"));
		sys_seek(fd, 0, SEEK_SET);
		sys_read(fd, data, strlen("hello world"));
		sys_close(fd);
		kprintf("FS: %s\n", data);
	}

}

void
user_init(void) {
	thread_t *thrA, *thrB, *thrC;

	thr_create_thread(1, &thrA, NULL, threadA, NULL);
	thr_create_thread(2, &thrB, NULL, threadB, NULL);
	thr_create_thread(3, &thrC, NULL, threadC, (void *)0x5a5a5a5a5a);
}
