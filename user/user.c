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
		thr_delay(500);
	}
}

void
threadB(void *arg) {

	while(1) {

		kprintf("threadB\n");
		thr_delay(1000);
	}
}

void
threadC(void *arg) {
	int fd;
	char data[64];
	char c[1];
	struct _stat s1, s2;
	struct _stat con_stat;

	kprintf("threadC\n");

	fd = fs_open("/test.txt", O_RDWR|O_CREATE);
	if ( fd >= 0 ) {
		
		fs_fstat(fd, &s1);
		fs_write(fd, "hello world", strlen("hello world"));
		fs_fstat(fd, &s2);

		fs_lseek(fd, 0, SEEK_SET);
		fs_read(fd, data, strlen("hello world"));

		fs_close(fd);
		kprintf("FS: size1:%u size2:%u %s\n", s1.size, s2.size, data);
	}

	fd = fs_open("/CON", O_RDWR);
	if ( fd >= 0 ) {

		fs_fstat(fd, &con_stat);
		kprintf("stat for CON: ino:%u mode:%u size:%u\n", 
		    con_stat.ino, con_stat.mode, con_stat.size);
		fs_write(fd, "hello console\n", strlen("hello console\n"));

		while( fs_read( fd, &c[0], 1) == 1 ) 
			fs_write( fd, &c[0], 1);

		fs_close(fd);
	}
}

void
user_init(void) {
	thread_t *thrA, *thrB, *thrC;
	thread_attr_t attr;

	memset(&attr, 0, sizeof(thread_attr_t));
	attr.prio = 2;
	thr_create_thread(1, &thrA, NULL, threadA, NULL);
	thr_create_thread(2, &thrB, NULL, threadB, NULL);
	thr_create_thread(3, &thrC, &attr, threadC, (void *)0x5a5a5a5a5a);
}
