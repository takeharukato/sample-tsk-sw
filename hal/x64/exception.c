/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Exception handler                                                 */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

#include <hal/exception.h>

/** トラップ名
 */
static const char *trapnames[] = {
	"Divide-by-zero Error",
	"Debug",
	"Non-maskable Interrupt",
	"Breakpoint",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack-Segment Fault",
	"General Protection Fault",
	"Page Fault",
	"Reserved",
	"x87 Floating-Point Exception",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point Exception",
	"Virtualization Exception",
	"Reserved",
	"Security Exception",
	"Reserved",
};

void
hal_handle_exception(exception_frame *exc) {
	int       i;
	uint64_t rf;
	
	kprintf("An exception occur:\n");
	if ( exc->trapno < X86_NR_EXCEPTIONS )
		kprintf("%s:\n", trapnames[exc->trapno]);
	else
		kprintf("UnknownTrap:\n", trapnames[exc->trapno]);

	kprintf("Trap: 0x%x(%d) ErrorCode: 0x%016lx\n", 
	    exc->trapno, exc->trapno, exc->errno);

	kprintf("RIP: 0x%02x:0x%016lx RSP: 0x%016lx\n",
	    (uint16_t)exc->cs, exc->rip,
	    ( ( (uintptr_t)exc->rsp) - sizeof(exception_frame) ) );

	kprintf("RFLAGS: 0x%016lx\n", exc->rflags);

	kprintf("RAX: 0x%016lx RBX: 0x%016lx RCX: 0x%016lx RDX: 0x%016lx\n",
	    exc->rax, exc->rbx, exc->rcx, exc->rdx);
	kprintf("RBP: 0x%016lx RSI: 0x%016lx RDI: 0x%016lx\n",
	    exc->rbp, exc->rsi, exc->rdi);
	kprintf("R8 : 0x%016lx R9 : 0x%016lx R10: 0x%016lx R11: 0x%016lx\n",
	    exc->r8, exc->r9, exc->r10, exc->r11);
	kprintf("R12: 0x%016lx R13: 0x%016lx R14: 0x%016lx R15: 0x%016lx\n",
	    exc->r12, exc->r13, exc->r14, exc->r15);
}

void
hal_common_trap_handler(exception_frame *exc){
	thread_info_t *ti;

	ti = hal_get_current_thread_info();

	if ( exc->trapno < X86_NR_EXCEPTIONS ) {
		
		ti_update_preempt_count(ti, THR_EXCCNT_SHIFT, 1);
		psw_enable_interrupt();
		hal_handle_exception(exc);
		psw_disable_interrupt();
		ti_update_preempt_count(ti, THR_EXCCNT_SHIFT, -1);

	} else {

		ti_update_preempt_count(ti, THR_IRQCNT_SHIFT, 1);
		irq_handle_irq(exc);
		ti_update_preempt_count(ti, THR_IRQCNT_SHIFT, -1);
	}

	sched_delay_disptach();  /*  遅延ディスパッチ  */

	return;
}
