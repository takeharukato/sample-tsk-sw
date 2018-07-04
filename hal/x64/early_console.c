/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Kernel console                                                    */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>
#include <hal/port.h>
#include <hal/uart.h>

static kconsole_t uart_console = KCONSOLE_INITILIZER(uart_console);

/** 一文字出力関数
    @param[in] ch 出力する文字
 */
void
uart8250_putc(int ch){
	psw_t psw;

	psw_disable_and_save_interrupt(&psw);

	while (!(in_port_byte(CON_COMBASE + UART_LSR) & UART_LSR_TXHOLD));
	out_port_byte(CON_COMBASE, (char) ch);

	psw_restore_interrupt(&psw);
}
void
uart8250_init(void){
	uint16_t baud_setting;

	baud_setting = UART_SERIAL_FREQ / UART_INTERNAL_BIAS / UART_DEFAULT_BAUD;
	out_port_byte(CON_COMBASE + UART_INTR, 0);  /*  disable interrupt  */
	out_port_byte(CON_COMBASE + UART_MCTRL, uart_mk_mctl(0)); 

	out_port_byte(CON_COMBASE + UART_LLAT, baud_setting & 0xff ); /* low */
	out_port_byte(CON_COMBASE + UART_HLAT, baud_setting >> 8);    /* high */
	out_port_byte(CON_COMBASE + UART_LCTRL, 0x03); /*  8 bit stop=0 Non-parity */

	out_port_byte(CON_COMBASE + UART_FIFO, 0x0);      /* disable FIFO */

	uart_console.putchar = uart8250_putc;
	register_kconsole(&uart_console);
}
