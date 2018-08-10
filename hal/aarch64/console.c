/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Console driver for PL011                                          */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

static off_t aarch64_console_read(inode *_ip, file_descriptor *_f, void *_dst, off_t _off, size_t _counts);
static off_t aarch64_console_write(inode *_ip, file_descriptor *_f, void *_dst, off_t _off, size_t _counts);

static device_driver console={
	.open = NULL,
	.read = aarch64_console_read,
	.write = aarch64_console_write,
	.blkrw = NULL,
	.close = NULL,	
};

volatile uint32_t * const UARTDR = (uint32_t *)UART_BASE;
volatile uint32_t * const UARTFR = (uint32_t *)(UART_BASE + 0x18);

off_t aarch64_console_read(inode *ip, file_descriptor *f, void *dst, off_t off, 
    size_t counts){
	size_t remains;
	char    r, *cp;
	
	for( cp = (char *)dst, remains = counts;remains > 0; --remains, ++cp ) {
		
		while( *UARTFR & 0x10 );

		r = (char)( *UARTDR );

		/* convert carrige return to newline */		
		*cp = ( r == '\r' ) ? ( '\n' ) : ( r );
	}

	return (off_t)counts;
}

off_t aarch64_console_write(inode *ip, file_descriptor *f, void *src, off_t off, 
    size_t counts){
	size_t remains;
	char    *cp;
	
	for( cp = (char *)src, remains = counts;remains > 0; --remains, ++cp ) {

		while( *UARTFR & 0x20 );
		*UARTDR = *cp;
	}

	return (off_t)counts;
}

void 
aarch64_console_init(void){
	
	register_device_driver(CONS_DEV, &console, NULL);
}
