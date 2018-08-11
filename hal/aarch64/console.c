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

off_t aarch64_console_read(inode *ip, file_descriptor *f, void *dst, off_t off, 
    size_t counts){
	size_t remains;
	char    r, *cp;

	for( cp = (char *)dst, remains = counts;remains > 0; --remains, ++cp ) {

		while( *UART_FR & 0x10 );
		r = (char)( *UART_DR );

		/* convert carrige return to newline */		
		*cp = ( r == '\r' ) ? ( '\n' ) : ( r );
	}

	return (off_t)counts;
}

off_t aarch64_console_write(inode *ip, file_descriptor *f, void *src, off_t off, 
    size_t counts){
	size_t remains;
	char       *cp;

	for( cp = (char *)src, remains = counts;remains > 0; --remains, ++cp ) {

		while( *UART_FR & 0x20 );
		*UART_DR = *cp;
	}

	return (off_t)counts;
}

void 
aarch64_console_init(void){

	*UART_ICR = 0x7FF;    // clear interrupts
	*UART_IBRD = 2;       // 115200 baud
	*UART_FBRD = 0xB;
	*UART_LCRH = 0b11<<5; // 8n1

	// enable Tx(0x100), Rx(0x200), and UART(0x001)
	*UART_CR = (0x1 << 8) | (0x1 << 9) | (0x1 << 0) ; 

	register_device_driver(CONS_DEV, &console, NULL);
}
