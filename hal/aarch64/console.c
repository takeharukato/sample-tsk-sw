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

static struct _aarch64_console{
	wait_queue waiters;
}aarch64_console;

static off_t 
aarch64_console_read(inode *ip, file_descriptor *f, void *dst, off_t off, 
    size_t counts){
	size_t   remains;
	char      r, *cp;
	wq_reason reason;
	psw_t        psw;

	for( cp = (char *)dst, remains = counts;remains > 0; --remains, ++cp ) {

		while( *UART_FR & UART_FR_RXFE ){

			psw_disable_and_save_interrupt(&psw);
			reason = wque_wait_on_queue(&aarch64_console.waiters);
			psw_restore_interrupt(&psw);
			kassert( reason == WQUE_REASON_WAKEUP);
		}
		r = (char)( *UART_DR );

		/* convert carrige return to newline */		
		*cp = ( r == '\r' ) ? ( '\n' ) : ( r );
	}

	return (off_t)counts;
}

static off_t
aarch64_console_write(inode *ip, file_descriptor *f, void *src, off_t off, 
    size_t counts){
	size_t remains;
	char       *cp;
	wq_reason reason;
	psw_t        psw;


	for( cp = (char *)src, remains = counts;remains > 0; --remains, ++cp ) {

		while( *UART_FR & UART_FR_TXFF ){
			
			psw_disable_and_save_interrupt(&psw);
			reason = wque_wait_on_queue(&aarch64_console.waiters);
			psw_restore_interrupt(&psw);
			kassert( reason == WQUE_REASON_WAKEUP);
		}
		*UART_DR = *cp;
	}

	return (off_t)counts;
}

static int 
aarch64_uart_handler(irq_no irq, struct _exception_frame *exc, void *private){
	psw_t psw;

	psw_disable_and_save_interrupt(&psw);
	wque_wakeup(&aarch64_console.waiters, WQUE_REASON_WAKEUP);
	psw_restore_interrupt(&psw);
	
	*UART_ICR = UART_CLR_ALL_INTR;  /* Clear all interrupts */

	return IRQ_HANDLED;
}

void 
aarch64_console_init(void){

	wque_init_wait_queue(&aarch64_console.waiters);

	*UART_ICR = UART_CLR_ALL_INTR;  /* Clear all interrupts */

	/*
	 * 115200 bits per seconds
	 */
	*UART_IBRD = UART_IBRD_VAL;
	*UART_FBRD = UART_FBRD_VAL;

	*UART_LCRH = UART_LCRH_WLEN8; /* 8bit non parity 1 stop bit */

	/* enable Tx(0x100), Rx(0x200), and UART(0x001) */
	*UART_CR = (UART_CR_TXE | UART_CR_RXE | UART_CR_UARTEN);

	irq_register_handler(AARCH64_UART_IRQ , 
	    IRQ_ATTR_NESTABLE | IRQ_ATTR_EXCLUSIVE | IRQ_ATTR_EDGE, 
	    2, NULL, aarch64_uart_handler);

	*UART_IMSC |= ( UART_IMSC_RXIM | UART_IMSC_TXIM );

	register_device_driver(CONS_DEV, &console, NULL);
}
