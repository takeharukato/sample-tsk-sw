/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  AArch64 Virtual Timer                                             */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

#include <hal/exception.h>
#include <hal/timer.h>

#define HZ_PER_US (1000000UL)

#define CNTV_CTL_ENABLE  (1 << 0)
#define CNTV_CTL_MASK    (1 << 1)
#define CNTV_CTL_START   (1 << 2)

static struct _aarch64_timer_config{
	uint64_t interval_us;
}aarch64_timer_config;

/* Generic Timer (Virtual Timer) register operations
 * See D10.6 Generic Timer registers in
 * ARM Architecture Reference Manual ARMv8, for ARMv8-A architecture profile
 */
static uint64_t
aarch64_get_vctcnt(void) {
	uint64_t val;

	__asm__ __volatile__("mrs %0, cntvct_el0\n\t" : "=r"(val));

	return val;
}

static uint64_t
clock_read_ctrl(void) {
	uint64_t cntv_ctrl;

	asm volatile ("isb; mrs %0, cntv_ctl_el0; isb;" 
	    : "=r"(cntv_ctrl) :: "memory");

	return cntv_ctrl;
}

static void
clock_write_ctrl(uint64_t cntv_ctrl) {

	asm volatile ("isb; msr cntv_ctl_el0, %0; isb;" 
	    :: "r"(cntv_ctrl) : "memory");
	return ;
}

static uint64_t
clock_read_counter(void){
	uint64_t cval;

	asm volatile("mrs %0, cntvct_el0" : "=r" (cval));

	return cval;
}

static uint64_t
clock_read_tval(void){
	uint64_t cntv_tval;

	asm volatile ("isb; mrs %0, cntv_tval_el0; isb;" : "=r"(cntv_tval)
	    :: "memory");
	return cntv_tval;
}

static void
clock_write_tval(uint64_t cntv_tval){

	asm volatile ("isb; msr cntv_tval_el0, %0; isb;" :: "r"(cntv_tval)
	    : "memory");
}

static void
stop_timer(void) {
	uint64_t cntv_ctl;

	cntv_ctl = clock_read_ctrl();
	cntv_ctl &= ~( CNTV_CTL_ENABLE | CNTV_CTL_START ); //disable
	cntv_ctl |=  CNTV_CTL_MASK; //mask
	clock_write_ctrl(cntv_ctl);
}

static void
start_timer(void) {
	uint64_t cntv_ctl;

	cntv_ctl = clock_read_ctrl();
	cntv_ctl |=  ( CNTV_CTL_ENABLE | CNTV_CTL_START ); //enable
	cntv_ctl &= ~CNTV_CTL_MASK; //mask
	clock_write_ctrl(cntv_ctl);
}

static void
reload_timer(uint64_t next_us){
	uint64_t diff;

	diff = next_us * ( aarch64_get_generic_timer_freq() / HZ_PER_US  );
	clock_write_tval(diff);
}

static int 
aarch64_timer_handler(irq_no irq, struct _exception_frame *exc, void *private){
	struct _aarch64_timer_config *cfg = (struct _aarch64_timer_config *)private;

	stop_timer();
	reload_timer(cfg->interval_us);
	start_timer();

	timer_update_uptime();
	timer_update_thread_time();

	return IRQ_HANDLED;
}

void
hal_init_timer(uint64_t timer_interval_ms){

	aarch64_timer_config.interval_us  = timer_interval_ms * 1000;
	stop_timer();
	reload_timer(aarch64_timer_config.interval_us);
	start_timer();
	
	irq_register_handler(AARCH64_TIMER_IRQ, 
	    IRQ_ATTR_NON_NESTABLE | IRQ_ATTR_EXCLUSIVE | IRQ_ATTR_EDGE, 
	    1, &aarch64_timer_config, aarch64_timer_handler);
}
