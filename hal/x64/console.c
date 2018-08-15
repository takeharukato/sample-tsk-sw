/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Console driver for NS8250/16550 UART                              */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>
#include <hal/port.h>
#include <hal/uart.h>

static off_t x64_console_read(inode *_ip, file_descriptor *_f, void *_dst, off_t _off, size_t _counts);
static off_t x64_console_write(inode *_ip, file_descriptor *_f, void *_dst, off_t _off, size_t _counts);

static device_driver console={
	.open = NULL,
	.read = x64_console_read,
	.write = x64_console_write,
	.blkrw = NULL,
	.close = NULL,	
};

static struct _x64_console{
	wait_queue rx_waiters;
	wait_queue tx_waiters;
}x64_console;

static off_t 
x64_console_read(inode *ip, file_descriptor *f, void *dst, off_t off, 
    size_t counts){
	size_t   remains;
	char      r, *cp;
	wq_reason reason;
	psw_t        psw;

	for( cp = (char *)dst, remains = counts;remains > 0; --remains, ++cp ) {

		while( !( in_port_byte(CON_COMBASE + UART_LSR) & UART_LSR_RXRDY ) ) {

			psw_disable_and_save_interrupt(&psw);
			reason = wque_wait_on_queue(&x64_console.rx_waiters);
			psw_restore_interrupt(&psw);
			kassert( reason == WQUE_REASON_WAKEUP);
		}
		r = (char)in_port_byte(CON_COMBASE);

		/* convert carrige return to newline */		
		*cp = ( r == '\r' ) ? ( '\n' ) : ( r );
	}

	return (off_t)counts;
}

static off_t
x64_console_write(inode *ip, file_descriptor *f, void *src, off_t off, 
    size_t counts){
	size_t remains;
	char       *cp;
	wq_reason reason;
	psw_t        psw;


	for( cp = (char *)src, remains = counts;remains > 0; --remains, ++cp ) {

		while( !( in_port_byte(CON_COMBASE + UART_LSR) & UART_LSR_TXHOLD ) ) {
			
			psw_disable_and_save_interrupt(&psw);
			reason = wque_wait_on_queue(&x64_console.tx_waiters);
			psw_restore_interrupt(&psw);
			kassert( reason == WQUE_REASON_WAKEUP);
		}
		out_port_byte(CON_COMBASE + UART_DAT, (uint8_t)*cp);
	}

	return (off_t)counts;
}

static int 
x64_uart_handler(irq_no irq, struct _exception_frame *exc, void *private){
	uint8_t           intr_id;
	psw_t                 psw;
	struct _x64_console  *inf;

	inf = (struct _x64_console *)private;

	psw_disable_and_save_interrupt(&psw);

	intr_id = uart_getiir(in_port_byte(CON_COMBASE + UART_IIR));

	if ( intr_id & UART_IIR_RDI )
		wque_wakeup(&inf->rx_waiters, WQUE_REASON_WAKEUP);

	if ( intr_id & UART_IIR_THRI )
		wque_wakeup(&inf->tx_waiters, WQUE_REASON_WAKEUP);

	psw_restore_interrupt(&psw);

	return IRQ_HANDLED;
}

void 
x64_console_init(void){
	uint16_t baud_setting;

	wque_init_wait_queue(&x64_console.rx_waiters);
	wque_init_wait_queue(&x64_console.tx_waiters);

	baud_setting = UART_SERIAL_FREQ / UART_INTERNAL_BIAS / UART_DEFAULT_BAUD;
	out_port_byte(CON_COMBASE + UART_INTR, 0);  /*  disable interrupt  */
	out_port_byte(CON_COMBASE + UART_MCTRL, uart_mk_mctl(0)); 

	out_port_byte(CON_COMBASE + UART_LLAT, baud_setting & 0xff ); /* low */
	out_port_byte(CON_COMBASE + UART_HLAT, baud_setting >> 8);    /* high */
	out_port_byte(CON_COMBASE + UART_LCTRL, 0x03); /*  8 bit stop=0 Non-parity */

	irq_register_handler(UART_COM_IRQ , 
	    IRQ_ATTR_NESTABLE | IRQ_ATTR_EXCLUSIVE | IRQ_ATTR_EDGE, 
	    0, &x64_console, x64_uart_handler);

	out_port_byte(CON_COMBASE + UART_FIFO, 0x0);  /* disable FIFO */
	out_port_byte(CON_COMBASE + UART_INTR, UART_INTR_RDA|UART_INTR_TXE);  /*  送受信割り込み使用  */
	out_port_byte(CON_COMBASE + UART_MCTRL, uart_mk_mctl(UART_MCTL_ENINTR)); /* 割込み有効化 */

	register_device_driver(CONS_DEV, &console, NULL);
}
