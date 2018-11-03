/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014-2018 Takeharu KATO                                 */
/*                                                                    */
/*  Local APIC                                                        */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>
#include <hal/hal.h>

#include <hal/i8254.h>
#include <hal/i8259.h>

//#define DEBUG_LAPIC_READ
//#define DEBUG_LAPIC_WRITE
//#define DEBUG_LAPIC_CALIBRATION

static lapic lapic_regs = {.paddr = LAPIC_DEFAULT_ADDR, .kaddr = (void *)PHY_TO_KERN_STRAIGHT(LAPIC_DEFAULT_ADDR)};

#define LAPIC_CALIBRATION_LOOP (10)
/* FIXME: Calibration 関連のデータ構造をまとめておくこと(lapic_regs) */
static uint64_t        tsc_per_us;
static uint64_t        tcr_per_us;
static int      calibration_count;

static uint32_t
lapic_read(uint32_t offset){
	uint32_t data;
	
	data = *((volatile uint32_t *)(lapic_regs.kaddr + offset));
#if defined(DEBUG_LAPIC_READ)
	kprintf("lapic_read: Addr=%p data=0x%x\n", 
	    (volatile uint32_t *)(lapic_regs.kaddr + offset),
	    data);
#endif  /*  DEBUG_LAPIC_READ  */
	return data;
}		

static void 
lapic_write(uint32_t offset, uint32_t data){

#if defined(DEBUG_LAPIC_WRITE)	
	kprintf("lapic_write: Addr=%p data=0x%x\n", 
	    (volatile uint32_t *)(lapic_regs.kaddr + offset),
	    data);
#endif  /*  DEBUG_LAPIC_WRITE  */
	*((volatile uint32_t *)(lapic_regs.kaddr + offset))=data;
}

static  uint32_t
lapic_errstatus(void){

	lapic_write(LAPIC_ESR, 0);
	return lapic_read(LAPIC_ESR);
}

static  void
lapic_eoi(void){

	lapic_write(LAPIC_EOI, 0);
	return;
}

static int
lapic_calibrate_spurious_handler(irq_no irq __attribute__((unused)), 
    struct _exception_frame *exc __attribute__((unused)), 
    void *private __attribute__((unused))){

	return IRQ_HANDLED;
}

static int
lapic_calibrate_handler(irq_no irq __attribute__((unused)), 
    struct _exception_frame *exc __attribute__((unused)), 
    void *private __attribute__((unused))){
	static uint64_t tsc1=0, tsc2=0;
	static uint32_t tcr1=0, tcr2=0;

	if (tsc1 == 0 ){

		tsc1 = rdtsc();
		tcr1 = lapic_read(LAPIC_TIMER_CCR);
	} else if (tsc2 == 0 ) {

		tsc2 = rdtsc();
		tcr2 = lapic_read(LAPIC_TIMER_CCR);
		
		tsc_per_us += tsc2 - tsc1;
		tcr_per_us += tcr1 - tcr2;

		--calibration_count;
		tsc1 = tsc2 = 0;
		tcr1 = tcr2 = 0;
	}

	return IRQ_HANDLED;
}

static int 
lapic_spurios_intr_handler(irq_no irq __attribute__((unused)), 
    struct _exception_frame *exc __attribute__((unused)), 
    void *private __attribute__((unused))){
	static unsigned int cnt;

	++cnt;
	if (cnt == 1 || (cnt % 100) == 0)
		kprintf("WARNING apic spurios interrupt(s) %d\n", cnt);

	return IRQ_HANDLED;
}

static int
lapic_error_intr_handler(irq_no irq __attribute__((unused)), 
    struct _exception_frame *exc __attribute__((unused)), 
    void *private __attribute__((unused))){
	static unsigned int cnt;
	uint32_t            err;

	++cnt;
	err = lapic_errstatus();
	if (cnt == 1 || (cnt % 100) == 0)
		kprintf("WARNING apic error (0x%x) interrupt(s) %d\n",
		       err, cnt);

	return IRQ_HANDLED;
}

static void
lapic_calibrate_clocks(uint32_t cpu){
	uint32_t   divisor;
	uint32_t lvtt, val;
	psw_t          psw;

	divisor = I8254_INPFREQ / 100;  /* 10ms timer */

	psw_disable_and_save_interrupt(&psw);
	
	out_port_byte(I8254_PORT_MODECNTL, I8254_CMD_INTERVAL_TIMER);
	out_port_byte(I8254_PORT_CHANNEL0, (uint8_t)( divisor & ~( (uint8_t)(0) ) ) );
	out_port_byte(I8254_PORT_CHANNEL0, (uint8_t)( (divisor >> 8) & ~( (uint8_t)(0) ) ) );
	
	irq_register_handler(X64_I8254_TIMER_IRQ, 
	    IRQ_ATTR_NON_NESTABLE | IRQ_ATTR_EXCLUSIVE | IRQ_ATTR_EDGE, 
	    0, NULL, lapic_calibrate_handler);

	irq_register_handler(X64_I8259_SPURIOUS_IRQ, 
	    IRQ_ATTR_NON_NESTABLE | IRQ_ATTR_EXCLUSIVE | IRQ_ATTR_EDGE, 
	    0, NULL, lapic_calibrate_spurious_handler);

	/*
	 * Set Initial count register to the highest value so it does not
	 * underflow during the testing period
	 * */
	val = 0xffffffff;
	lapic_write (LAPIC_TIMER_ICR, val);

	/* Set Current count register */
	val = 0;
	lapic_write (LAPIC_TIMER_CCR, val);

	lvtt = lapic_read(LAPIC_TIMER_DCR) & ~0x0b;
	 /* Set Divide configuration register to 1 */
	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/*
	 * mask the APIC timer interrupt in the LVT Timer Register so that we
	 * don't get an interrupt upon underflow which we don't know how to
	 * handle right know. If underflow happens, the system will not continue
	 * as something is wrong with the clock IRQ 0 and we cannot calibrate
	 * the clock which mean that we cannot run processes
	 */
	lvtt = lapic_read (LAPIC_LVTTR);
	lvtt |= LAPIC_LVTT_MASK;
	lapic_write (LAPIC_LVTTR, lvtt);

	/*
	 * Calibration
	 */
	for(calibration_count = LAPIC_CALIBRATION_LOOP; calibration_count > 0; )
		psw_enable_interrupt();
	
	psw_restore_interrupt(&psw);
	irq_unregister_handler(X64_I8254_TIMER_IRQ,  lapic_calibrate_handler);
	irq_unregister_handler(X64_I8259_SPURIOUS_IRQ, lapic_calibrate_spurious_handler);

	/* Note we used 10ms timer */
	tsc_per_us = (tsc_per_us / LAPIC_CALIBRATION_LOOP ) / 10000;
	tcr_per_us = (tcr_per_us / LAPIC_CALIBRATION_LOOP ) / 10000;
#if defined(DEBUG_LAPIC_CALIBRATION)
	kprintf("Calibration tsc: %d tsc/us tcr: %d count/us\n", tsc_per_us, tcr_per_us);
#endif  /*  DEBUG_LAPIC_CALIBRATION  */
}

static void 
lapic_enable_irq(struct _irq_ctrlr *ctrlr, irq_no no){
	uint32_t val;

	kassert( ( LAPIC_INT_VECTOR_MIN <= no) &&
		 ( no <= LAPIC_INT_VECTOR_MAX ) );

	switch(no) {
	case LAPIC_TIMER_INT_VECTOR:
		val = lapic_read(LAPIC_LVTTR);
		lapic_write(LAPIC_LVTTR, val & (~LAPIC_LVTT_MASK) );
		break;
	case LAPIC_ERROR_INT_VECTOR:
		val = lapic_read(LAPIC_LVTER);
		lapic_write(LAPIC_LVTER, val & (~LAPIC_ICR_INT_MASK));
		break;
	case LAPIC_SPURIOUS_INT_VECTOR:
		break;
	default:
		break;
	}
}

static void 
lapic_disable_irq(struct _irq_ctrlr *ctrlr, irq_no no){
	uint32_t val;

	kassert( ( LAPIC_INT_VECTOR_MIN <= no) &&
		 ( no <= LAPIC_INT_VECTOR_MAX ) );

	switch(no) {
	case LAPIC_TIMER_INT_VECTOR:
		val = lapic_read(LAPIC_LVTTR);
		lapic_write(LAPIC_LVTTR, val | LAPIC_LVTT_MASK);
		break;
	case LAPIC_ERROR_INT_VECTOR:
		val = lapic_read(LAPIC_LVTER);
		lapic_write(LAPIC_LVTER, val | LAPIC_ICR_INT_MASK);
		break;
	case LAPIC_SPURIOUS_INT_VECTOR:
		break;
	default:
		break;
	}
}

static void 
lapic_eoi_irq(struct _irq_ctrlr *ctrlr __attribute__((unused)), 
	      irq_no no __attribute__((unused))){

	lapic_eoi();
}

static int 
lapic_config_irq(struct _irq_ctrlr *ctrlr, 
		 irq_no irq, irq_attr attr, irq_prio prio){

	kassert( ( LAPIC_INT_VECTOR_MIN <= irq) &&
		 ( irq <= LAPIC_INT_VECTOR_MAX ) );

	return 0;
}

static int 
init_lapic(struct _irq_ctrlr *ctrlr){

	return 0;
}

static void 
finalize_lapic(struct _irq_ctrlr *ctrlr){
	uint32_t val, lvt_nr;

	lvt_nr = (lapic_read(LAPIC_VERSION)>>16) & 0xFF;

	val = lapic_read(LAPIC_LINT0);
	lapic_write(LAPIC_LINT0, val | LAPIC_ICR_INT_MASK);

	val = lapic_read(LAPIC_LINT1);
	lapic_write(LAPIC_LINT1, val | LAPIC_ICR_INT_MASK);

	val = lapic_read(LAPIC_LVTER);
	lapic_write(LAPIC_LVTER, val | LAPIC_ICR_INT_MASK);

	val = lapic_read(LAPIC_LVTTR);
	lapic_write(LAPIC_LVTTR, val | LAPIC_LVTT_MASK);


	if (lvt_nr >= 4) { /* Mask LAPIC Thermal Sensor Interrupt */

		val = lapic_read(LAPIC_LVTTMR);
		lapic_write(LAPIC_LVTTMR, val | LAPIC_ICR_INT_MASK);
	}

	if (lvt_nr >= 5) { /* Mask Performance Monitoring Interrupt */

		val = lapic_read(LAPIC_LVTPCR);
		lapic_write(LAPIC_LVTPCR, val | LAPIC_ICR_INT_MASK);
	}

	lapic_write(LAPIC_TPR, 0x0); 	/* set the highest priority for ever */
	lapic_errstatus();              /* Clear error status  */

	val = lapic_read(LAPIC_SIVR);
	val &= ~LAPIC_ENABLE;
	val |= LAPIC_FOCUS_DISABLED;
	lapic_write(LAPIC_SIVR, val);
}

static int 
lapic_find_pending_irq(struct _exception_frame *exc, irq_no *irqp){

	kassert( ( LAPIC_INT_VECTOR_MIN <= exc->trapno ) &&
		 ( exc->trapno <= LAPIC_INT_VECTOR_MAX ) );

	switch(exc->trapno) {
	case LAPIC_TIMER_INT_VECTOR:
		break;
	case LAPIC_ERROR_INT_VECTOR:
		break;
	case LAPIC_SPURIOUS_INT_VECTOR:
		break;
	default:
		return IRQ_NOT_FOUND;
	}

	*irqp = exc->trapno;

	return IRQ_FOUND;
}

static irq_finder x64_lapic_find_irq = {
	__LIST_ENT_INITIALIZER(x64_lapic_find_irq.link),
	lapic_find_pending_irq
};

static irq_ctrlr lapic_ctrlr = {
	.config_irq = lapic_config_irq,
	.enable_irq = lapic_enable_irq,
	.disable_irq = lapic_disable_irq,
	.eoi = lapic_eoi_irq,
	.initialize = init_lapic,
	.finalize = finalize_lapic,
	.private = NULL
};

void 
x64_lapic_set_timer_periodic(const uint32_t freq_us){
	uint32_t lvtt;

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as periodic */
	lvtt = LAPIC_LVTT_TM | LAPIC_TIMER_INT_VECTOR;
	lapic_write(LAPIC_LVTTR, lvtt);

	lapic_write(LAPIC_TIMER_ICR, freq_us * tcr_per_us);
	
}
void
x64_lapic_set_timer_one_shot(const uint32_t usec){
	uint32_t lvtt;

	lapic_write(LAPIC_TIMER_ICR, usec * tcr_per_us);

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as one-shot */
	lvtt = LAPIC_TIMER_INT_VECTOR;
	lapic_write(LAPIC_LVTTR, lvtt);
}

int
x64_lapic_enable(uint32_t cpu) {
	uint64_t msr;
	uint32_t val, lvt_nr;

	msr = rdmsr(X64_MSR_APIC_BASE);
	msr |= (1 << X64_MSR_APIC_BASE_ENABLE_BIT);
	wrmsr(X64_MSR_APIC_BASE, msr);

	lapic_write(LAPIC_TPR, 0x0); 	/* set the highest priority for ever */
	lapic_errstatus();              /* Clear error status  */

	/* Enable Local APIC and set the spurious vector to 0xff. */
	val = lapic_read(LAPIC_SIVR);
	val |= LAPIC_ENABLE | LAPIC_SPURIOUS_INT_VECTOR;
	val &= ~LAPIC_FOCUS_DISABLED;
	lapic_write(LAPIC_SIVR, val);
	lapic_read(LAPIC_SIVR);

	irq_register_handler(LAPIC_SPURIOUS_INT_VECTOR, 
	    IRQ_ATTR_NON_NESTABLE | IRQ_ATTR_EXCLUSIVE | IRQ_ATTR_EDGE, 
	    0, NULL, lapic_spurios_intr_handler);

	/*
	 * Setup error interrupt vector 
	 */
	val = lapic_read(LAPIC_LVTER);
	val |= LAPIC_ERROR_INT_VECTOR;
	val &= ~ LAPIC_ICR_INT_MASK;
	lapic_write(LAPIC_LVTER, val);
	lapic_read(LAPIC_LVTER);

	irq_register_handler(LAPIC_ERROR_INT_VECTOR, 
	    IRQ_ATTR_NON_NESTABLE | IRQ_ATTR_EXCLUSIVE | IRQ_ATTR_EDGE, 
	    0, NULL, lapic_error_intr_handler);

	lapic_eoi();

	/* Program Logical Destination Register. */
	val = lapic_read(LAPIC_LDR) & ~0xFF000000;
	val |= (cpu & 0xFF) << 24;
	lapic_write(LAPIC_LDR, val);

	/* Program Destination Format Register for Flat mode. */
	val = lapic_read(LAPIC_DFR) | 0xF0000000;
	lapic_write (LAPIC_DFR, val);

	val = lapic_read (LAPIC_LVTER) & 0xFFFFFF00;
	lapic_write (LAPIC_LVTER, val);

	/*
	 * Obtain the number of LVTs via Local APIC Version Register
	 */
	lvt_nr = (lapic_read(LAPIC_VERSION)>>16) & 0xFF;

	if (lvt_nr >= 4) { /* Mask LAPIC Thermal Sensor Interrupt */

		val = lapic_read(LAPIC_LVTTMR);
		lapic_write(LAPIC_LVTTMR, val | LAPIC_ICR_INT_MASK);
	}

	if (lvt_nr >= 5) { /* Mask Performance Monitoring Interrupt */

		val = lapic_read(LAPIC_LVTPCR);
		lapic_write(LAPIC_LVTPCR, val | LAPIC_ICR_INT_MASK);
	}

	/*
	 * setup TPR to allow all interrupts. 
	 */
	val = lapic_read (LAPIC_TPR); 
	lapic_write (LAPIC_TPR, val & ~0xFF); /* accept all interrupts */

	lapic_read (LAPIC_SIVR);
	lapic_eoi();

	// Setup Local APIC timer
	lapic_calibrate_clocks(cpu);
	
	return 0;
}

int
x64_lapic_init(void){
	int        i;
	uint64_t msr;

	memset(&lapic_regs, 0, sizeof(lapic));
	
	msr = rdmsr(X64_MSR_APIC_BASE);
	lapic_regs.paddr = ( msr & LAPIC_ADDR_MASK );
	lapic_regs.kaddr = (void *)HIGH_IO_TO_KERN_STRAIGHT(lapic_regs.paddr);
	x64_lapic_enable(0);

	irq_register_pending_irq_finder(&x64_lapic_find_irq);
	for(i = LAPIC_INT_VECTOR_MIN; LAPIC_INT_VECTOR_MIN >= i; ++i) 
		irq_register_ctrlr(i, &lapic_ctrlr);

	return 0;
}
