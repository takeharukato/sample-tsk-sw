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
	*UART_DR = (unsigned int)(ch); /* Transmit char */
	psw_restore_interrupt(&psw);
}

void
aarch64_uart_init(void) {

	uart_console.putchar = aarch64_kputchar;
	register_kconsole(&uart_console);
}
