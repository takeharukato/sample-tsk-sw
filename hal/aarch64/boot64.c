/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  64bit boot code                                                   */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

#include <hal/hal.h>
#include <hal/timer.h>

static kconsole_t uart_console = KCONSOLE_INITILIZER(uart_console);

extern void kputchar(int ch);
extern void *bsp_stack;

#define CHECK_FLAG(flags,bit)   \
	((flags) & (1 << (bit))) /* Check if the bit BIT in FLAGS is set. */

/** パニック関数
 */
static void
boot_panic(const char *string) {

	kprintf ("boot panic : %s \n", string);
	while(1);
}

/** スレッドなどの初期化後のアーキ依存初期化処理
 */
void
hal_kernel_init(void) {
	thread_t *idle;
	thread_attr_t *attrp;

	idle = idle_refer_idle_thread();
	attrp = &idle->attr;
	attrp->stack_top = &bsp_stack;
	attrp->stack_size = STACK_SIZE;

	aarch64_init_interrupt();
	hal_init_timer(CONFIG_TIMER_INTERVAL_MS);
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
