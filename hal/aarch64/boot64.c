/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  64bit boot code                                                   */
/*                                                                    */
/**********************************************************************/
#include <stdint.h>

#include "kern/kconsole.h"
#include "kern/main.h"
#include "kern/printf.h"

#include "hal/hal.h"

static kconsole_t uart_console = KCONSOLE_INITILIZER(uart_console);

extern void kputchar(int ch);

#define CHECK_FLAG(flags,bit)   \
	((flags) & (1 << (bit))) /* Check if the bit BIT in FLAGS is set. */

/** パニック関数
 */
static void
boot_panic(const char *string) {

	kprintf ("boot panic : %s \n", string);
	while(1);
}
/** 64bit モードでのブートアップ
 */
void 
boot_main(void) {

	uart_console.putchar = kputchar;
	register_kconsole(&uart_console);

	kprintf("boot\n");

	kern_init();
	while(1);
}
