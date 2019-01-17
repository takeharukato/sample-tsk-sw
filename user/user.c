/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  User tasks                                                        */
/*                                                                    */
/**********************************************************************/
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#include <kern/kernel.h>

thread_t *thrA, *thrB, *thrC;

int
uprintf(const char *fmt, ...) {
	va_list          ap;
	size_t          len;
	char buf[PRFBUFLEN];

	va_start(ap, fmt);
	len = kvprintf(PRFBUFLEN, buf, fmt, 10, ap);
	va_end(ap);
	buf[len] = '\0';

	fs_write(1, buf, len);	

	return len;
}

void
cls(void) {

	uprintf("\033[2J");
}

void
locate(int x, int y) {

	uprintf("\033[%d;%dH", y, x);
}

void
threadA(void *arg) {
	static int cnt;

#if !defined(CONFIG_HAL)	
	kprintf("threadA=0x%lx\n", thrA);
#endif
	while(1) {

#if defined(CONFIG_HAL)
		locate(0, 0);
		uprintf("threadA=0x%lx\n", thrA);
		uprintf("threadA[%d]\n", ++cnt);
#else
		kprintf("threadA\n");
#endif  /*  CONFIG_HAL  */
		thr_delay(1000);
	}
}

void
threadB(void *arg) {
	static int cnt;

#if !defined(CONFIG_HAL)	
	kprintf("threadB=0x%lx\n", thrB);
#endif
	while(1) {

#if defined(CONFIG_HAL)
		locate(0, 3);
		uprintf("threadB=0x%lx\n", thrB);
		uprintf("threadB[%d]\n", ++cnt);
#else
		kprintf("threadB\n");
#endif  /* CONFIG_HAL  */
		thr_delay(500);
	}
}

void
threadC(void *arg) {
#if defined(CONFIG_HAL)
	int fd;
	char data[64];
	char c[1];
	struct _stat s1, s2;
	struct _stat con_stat;

	locate(0, 5);
	kprintf("threadC=0x%lx\n", thrC);

	fd = fs_open("/test.txt", O_RDWR|O_CREATE);
	if ( fd >= 0 ) {
		
		fs_fstat(fd, &s1);
		fs_write(fd, "hello world", strlen("hello world")+1);
		fs_fstat(fd, &s2);

		fs_lseek(fd, 0, SEEK_SET);
		fs_read(fd, data, strlen("hello world")+1);

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
#else
	kprintf("threadC\n");
#endif  /* CONFIG_HAL  */
}

void
user_init(void) {
	thread_attr_t attr;
	int  i;

	cls();
	for( i = 0; 50 > i; ++i)
		kprintf("\n");

	memset(&attr, 0, sizeof(thread_attr_t));
	attr.prio = 2;
	thr_create_thread(1, &thrA, NULL, threadA, NULL);
	thr_create_thread(2, &thrB, NULL, threadB, NULL);
	thr_create_thread(3, &thrC, &attr, threadC, (void *)0x5a5a5a5a5a);
}
