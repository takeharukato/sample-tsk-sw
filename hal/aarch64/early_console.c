/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  kernel console for PL011                                          */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

#include <hal/board.h>

static kconsole_t uart_console = KCONSOLE_INITILIZER(uart_console);
 
/** 一文字出力関数
    @param[in] ch 出力する文字
 */
void
aarch64_kputchar(int ch){
	psw_t psw;

	psw_disable_and_save_interrupt(&psw);
	while( *UART_FR & UART_FR_TXFF );
	*UART_DR = (unsigned int)(ch); /* Transmit char */
	psw_restore_interrupt(&psw);
}

void
aarch64_uart_init(void) {

	*UART_ICR = UART_CLR_ALL_INTR;  /* Clear all interrupts */

	/*
	 * 115200 bits per seconds
	 */
	*UART_IBRD = UART_IBRD_VAL;
	*UART_FBRD = UART_FBRD_VAL;

	*UART_LCRH = UART_LCRH_WLEN8; /* 8bit non parity 1 stop bit */

	/* enable Tx(0x100), and UART(0x001) */
	*UART_CR = (UART_CR_TXE | UART_CR_UARTEN);

	uart_console.putchar = aarch64_kputchar;
	register_kconsole(&uart_console);
}
