/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*                                                                    */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Arm Generic Interrupt Controller(PL390)                           */
/*  This code derived from GIC driver in TOPPERS/ASP kernel.          */
/*  See: https://www.toppers.jp/asp-kernel.html                       */
/**********************************************************************/

/* The above copyright holders grant permission gratis to use,           */
/* duplicate, modify, or redistribute (hereafter called use) this        */
/* software (including the one made by modifying this software),         */
/* provided that the following four conditions (1) through (4) are       */
/* satisfied.                                                            */
/* (1) When this software is used in the form of source code, the above  */
/*     copyright notice, this use conditions, and the disclaimer shown   */
/*     below must be retained in the source code without modification.   */
/* (2) When this software is redistributed in the forms usable for the   */
/*     development of other software, such as in library form, the above */
/*     copyright notice, this use conditions, and the disclaimer shown   */
/*     below must be shown without modification in the document provided */
/*     with the redistributed software, such as the user manual.         */
/* (3) When this software is redistributed in the forms unusable for the */
/*     development of other software, such as the case when the software */
/*     is embedded in a piece of equipment, either of the following two  */
/*     conditions must be satisfied:                                     */
/*   (a) The above copyright notice, this use conditions, and the        */
/*       disclaimer shown below must be shown without modification in    */
/*       the document provided with the redistributed software, such as  */
/*       the user manual. */
/*   (b) How the software is to be redistributed must be reported to the */
/*       TOPPERS Project according to the procedure described            */
/*       separately.                                                     */
/* (4) The above copyright holders and the TOPPERS Project are exempt    */
/*     from responsibility for any type of damage directly or indirectly */
/*     caused from the use of this software and are indemnified by any   */
/*     users or end users of this software from any and all causes of    */
/*     action whatsoever.                                                */
/* THIS SOFTWARE IS PROVIDED "AS IS." THE ABOVE COPYRIGHT HOLDERS AND    */
/* THE TOPPERS PROJECT DISCLAIM ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, ITS APPLICABILITY TO A PARTICULAR      */
/* PURPOSE. IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS AND THE        */
/* TOPPERS PROJECT BE LIABLE FOR ANY TYPE OF DAMAGE DIRECTLY OR          */
/* INDIRECTLY CAUSED FROM THE USE OF THIS SOFTWARE.                      */

#include <kern/kernel.h>

#include <hal/hal.h>

#include <hal/gic-pl390.h>

//#define INSTALL_TEST_HANDLER

static int gic_pl390_config_irq(struct _irq_ctrlr *_ctrlr __attribute__((unused)), 
				irq_no _irq, irq_attr _attr, irq_prio _prio);
static void gic_pl390_enable_irq(struct _irq_ctrlr *_ctrlr __attribute__((unused)),
				 irq_no _irq);
static void gic_pl390_disable_irq(struct _irq_ctrlr *_ctrlr __attribute__((unused)),
				 irq_no _irq);
static void gic_pl390_get_priority(struct _irq_ctrlr *_ctrlr __attribute__((unused)),
				   irq_prio *_prio);
static void gic_pl390_set_priority(struct _irq_ctrlr *_ctrlr __attribute__((unused)), 
				   irq_prio _prio);
static void gic_pl390_eoi(struct _irq_ctrlr *_ctrlr __attribute__((unused)), irq_no _irq);
static int gic_pl390_initialize(struct _irq_ctrlr *_ctrlr);
static void gic_pl390_finalize(struct _irq_ctrlr *_ctrlr);

static int gic_pl390_find_pending_irq(struct _exception_frame *_exc __attribute__((unused)),
				      irq_no *_irqp);

static irq_ctrlr arm_gic_ctrlr = {
	.config_irq = gic_pl390_config_irq,
	.enable_irq = gic_pl390_enable_irq,
	.disable_irq = gic_pl390_disable_irq,
	.get_priority = gic_pl390_get_priority,
	.set_priority = gic_pl390_set_priority,
	.eoi = gic_pl390_eoi,
	.initialize = gic_pl390_initialize,
	.finalize = gic_pl390_finalize,
	.private = NULL
};

static irq_finder arm_gic_find_irq = {
	__LIST_ENT_INITIALIZER(arm_gic_find_irq.link),
	gic_pl390_find_pending_irq
};

/** Initialize GIC Controller
 * ARM Generic Interrupt Controller Architecture Specification version 1.0
 * Architecture Specification
 */
static int
init_gicc(void) {
	uint32_t pending_irq;

	/*
	 *  Disable CPU interface
	 */
	*REG_GIC_GICC_ICR = GICC_ICR_DISABLE;

	/*
	 *  Set the priority level as the lowest priority.
	 */
	/* Note: Higher priority corresponds to a lower Priority field value in the GIC_PMR.
	 * In addition to this, writing 255 to the GICC_PMR always sets it to the 
	 * largest supported priority field value.
	 * See. ARM Generic Interrupt Controller Architecture Specification version 2.0
	 * 3.3 Interrupt prioritization and 3.3.2 Priority masking
	 * for each issue.
	 */
	*REG_GIC_GICC_PMR = GICC_PMR_PRIO_MIN;

	/*
	 *  Handle all of interrupts in a single group.
	 */
	*REG_GIC_GICC_BPR = GICC_BPR_NO_GROUP;

	/*
	 *  Clear all of the active interrupts
	 */
	for(pending_irq = ( *REG_GIC_GICC_IAR & GICC_IAR_INTR_IDMASK ); 
	    ( pending_irq != GICC_IAR_SPURIOUS_INTR );
	    pending_irq = ( *REG_GIC_GICC_IAR & GICC_IAR_INTR_IDMASK ) )
		*REG_GIC_GICC_EOI = *REG_GIC_GICC_IAR;
	/*
	 * Enable CPU interface
	 */
	*REG_GIC_GICC_ICR = GICC_ICR_ENABLE;
	
	return 0;
}

static void
finalize_gicc(void) {

	/*
	 *  Disable CPU interface
	 */
	*REG_GIC_GICC_ICR = GICC_ICR_DISABLE;
}

static int
init_gicd(void) {
	int32_t	      i;
	int32_t	regs_nr;

	/*
	 *  Diable distributor
	 */
	*REG_GIC_GICD_CTLR = GIC_GICD_CTLR_DISABLE;

	/*
	 *  Disable all IRQs
	 */
	regs_nr = (GIC_INT_MAX + GIC_GICD_INT_PER_REG - 1) / GIC_GICD_INT_PER_REG;
	for (i = 0; regs_nr > i; ++i) 
		*REG_GIC_GICD_ICENABLER(i) = ~((uint32_t)(0)); 

	/*
	 *  Clear all pending IRQs
	 */
	regs_nr = (GIC_INT_MAX + GIC_GICD_INT_PER_REG - 1) / GIC_GICD_INT_PER_REG;
	for (i = 0; regs_nr > i; ++i) 
		*REG_GIC_GICD_ICPENDR(i) = ~((uint32_t)(0));

	/*
	 *  Set all of interrupt priorities as the lowest priority.
	 */
	regs_nr = ( GIC_INT_MAX + GIC_GICD_IPRIORITY_PER_REG - 1) / 
		GIC_GICD_IPRIORITY_PER_REG ;
	for (i = 0; regs_nr > i; i++)
		*REG_GIC_GICD_IPRIORITYR(i) = ~((uint32_t)(0));

	/*
	 *  Set target of all of shared peripherals to processor 0.
	 */
	for (i = GIC_INTNO_SPI0 / GIC_GICD_ITARGETSR_PER_REG;
	     ( (GIC_INT_MAX + (GIC_GICD_ITARGETSR_PER_REG - 1) ) / 
		 GIC_GICD_ITARGETSR_PER_REG ) > i; ++i) 
		*REG_GIC_GICD_ITARGETSR(i) = 
			(uint32_t)GIC_GICD_ITARGETSR_CORE0_TARGET_BMAP;

	/*
	 *  Set trigger type for all peripheral interrupts level triggered.
	 */
	for (i = GIC_INTNO_PPI0 / GIC_GICD_ICFGR_PER_REG;
	     (GIC_INT_MAX + (GIC_GICD_ICFGR_PER_REG - 1)) / GIC_GICD_ICFGR_PER_REG > i; ++i)
		*REG_GIC_GICD_ICFGR(i) = 0;

	/*
	 *  Enable distributor
	 */
	*REG_GIC_GICD_CTLR = GIC_GICD_CTLR_ENABLE;
	
	return 0;
}

/** Finalize GICD
 */
static void
finalize_gicd(void) {

	/*
	 *  Disable distributor
	 */
	*REG_GIC_GICD_CTLR = GIC_GICD_CTLR_DISABLE;
}

/** Disable IRQ
    @param[in] irq IRQ number
 */
static void
gicd_disable_int(irq_no irq) {

	*REG_GIC_GICD_ICENABLER( (irq / GIC_GICD_ICENABLER_PER_REG) ) = 
		1U << ( irq % GIC_GICD_ICENABLER_PER_REG );
}

/** Enable IRQ
    @param[in] irq IRQ number
 */
static void
gicd_enable_int(irq_no irq) {

	*REG_GIC_GICD_ISENABLER( (irq / GIC_GICD_ISENABLER_PER_REG) ) =
		1U << ( irq % GIC_GICD_ISENABLER_PER_REG );
}

/** Clear a pending interrupt
    @param[in] irq IRQ number
 */
static void
gicd_clear_pending(irq_no irq) {

	*REG_GIC_GICD_ICPENDR( (irq / GIC_GICD_ICPENDR_PER_REG) ) = 
		1U << ( irq % GIC_GICD_ICPENDR_PER_REG );
}

/** Set interrupt pending
    @param[in] irq IRQ number
 */
static void
gicd_set_pending(irq_no irq) {

	*REG_GIC_GICD_ISPENDR( (irq / GIC_GICD_ISPENDR_PER_REG) ) =
		1U << ( irq % GIC_GICD_ISPENDR_PER_REG );
}

/** Probe pending interrupt
    @param[in] irq IRQ number
 */
static int
gicd_probe_pending(irq_no irq) {
	int is_pending;

	is_pending = ( *REG_GIC_GICD_ISPENDR( (irq / GIC_GICD_ISPENDR_PER_REG) ) &
	    ( 1U << ( irq % GIC_GICD_ISPENDR_PER_REG ) ) );

	return ( is_pending != 0 );
}

/** Set an interrupt target processor
    @param[in] irq IRQ number
    @param[in] p   Target processor mask
    0x1 processor 0
    0x2 processor 1
    0x4 processor 2
    0x8 processor 3
 */
static void
gicd_set_target(irq_no irq, uint32_t p){
	uint32_t  shift;
	uint32_t    reg;

	shift = (irq % GIC_GICD_ITARGETSR_PER_REG) * GIC_GICD_ITARGETSR_SIZE_PER_REG;

	reg = *REG_GIC_GICD_ITARGETSR(irq / GIC_GICD_ITARGETSR_PER_REG);
	reg &= ~( ((uint32_t)(0xff)) << shift);
	reg |= (p << shift);
	*REG_GIC_GICD_ITARGETSR(irq / GIC_GICD_ITARGETSR_PER_REG) = reg;
}

/** Set an interrupt priority
    @param[in] irq  IRQ number
    @param[in] prio Interrupt priority in Arm specific expression
 */
static void
gicd_set_priority(irq_no irq, uint32_t prio){
	uint32_t  shift;
	uint32_t    reg;

	shift = (irq % GIC_GICD_IPRIORITY_PER_REG) * GIC_GICD_IPRIORITY_SIZE_PER_REG;
	reg = *REG_GIC_GICD_IPRIORITYR(irq / GIC_GICD_IPRIORITY_PER_REG);
	reg &= ~(((uint32_t)(0xff)) << shift);
	reg |= (prio << shift);
	*REG_GIC_GICD_IPRIORITYR(irq / GIC_GICD_IPRIORITY_PER_REG) = reg;
}

/** Configure IRQ line
    @param[in] irq     IRQ number
    @param[in] config  Configuration value for GICD_ICFGR
 */
static void
gicd_config(irq_no irq, unsigned int config){
	uint32_t	shift; 
	uint32_t	  reg;

	shift = (irq % 16) * 2; /* GICD_ICFGR has 16 fields, each field has 2bits. */

	reg = *REG_GIC_GICD_ICFGR( irq / 16 );

	reg &= ~( ( (uint32_t)(0x03) ) << shift );  /* Clear the field */
	reg |= ( ( (uint32_t)config ) << shift );  /* Set the value to the field correponding to irq */
	*REG_GIC_GICD_ICFGR( irq / 16 ) = reg;
}

/** Configure IRQ line for GIC
    @param[in] ctrlr   IRQ controller information
    @param[in] irq     IRQ number
    @param[in] attr    IRQ attribution
    @param[in] prio    Interrupt priority (Architecture Common Expression)
      0 ... The highest
     15 ... The lowest
 */
static int 
gic_pl390_config_irq(struct _irq_ctrlr *ctrlr __attribute__((unused)), 
    irq_no irq, irq_attr attr, irq_prio prio){

	gic_pl390_disable_irq(ctrlr, irq);

	if ( attr & IRQ_ATTR_EDGE ) {

		gicd_config(irq, GIC_GICD_ICFGR_EDGE);
		gicd_clear_pending(irq);
	} else {

		gicd_config(irq, GIC_GICD_ICFGR_LEVEL);
	}
	gicd_set_priority(irq, (prio & GIC_PRI_MASK) << GIC_PRI_SHIFT );  /* Set priority */
	gicd_set_target(irq, 0x1);  /* processor 0 */

	return 0;
}

/** Enable IRQ line for GIC
    @param[in] ctrlr   IRQ controller information
    @param[in] irq     IRQ number
 */
static void 
gic_pl390_enable_irq(struct _irq_ctrlr *ctrlr __attribute__((unused)), irq_no irq){

	gicd_enable_int(irq);
}

/** Disable IRQ line for GIC
    @param[in] ctrlr   IRQ controller information
    @param[in] irq     IRQ number
 */
static void
gic_pl390_disable_irq(struct _irq_ctrlr *ctrlr __attribute__((unused)), irq_no irq) {

	gicd_disable_int(irq);
}

/** Get priority level
    @param[out] prio The address where the current interrupt priority will be stored.
 */
static void
gic_pl390_get_priority(struct _irq_ctrlr *ctrlr __attribute__((unused)), irq_prio *prio) {
	uint32_t cur_prio;

	cur_prio = aarch64_get_intr_priority();
	*prio = cur_prio & 0xff;
}

/** Set priority level
    @param[in] prio interrupt priority to be set.
 */
static void 
gic_pl390_set_priority(struct _irq_ctrlr *ctrlr __attribute__((unused)), irq_prio prio) {

	aarch64_set_intr_priority(prio & 0xff);
}

/** Send End of Interrupt to IRQ line for GIC
    @param[in] ctrlr   IRQ controller information
    @param[in] irq     IRQ number
 */
static void
gic_pl390_eoi(struct _irq_ctrlr *ctrlr __attribute__((unused)), irq_no irq) {

	gicd_clear_pending(irq);
}

/** Initialize GIC IRQ controller
    @param[in] ctrlr   IRQ controller information
 */
static int
gic_pl390_initialize(struct _irq_ctrlr *ctrlr){

	init_gicd();
	init_gicc();
	ctrlr->private = NULL;
	
	return 0;
}

/** Finalize GIC IRQ controller
    @param[in] ctrlr   IRQ controller information
 */
static void
gic_pl390_finalize(struct _irq_ctrlr *ctrlr){

	finalize_gicc();
	finalize_gicd();
	ctrlr->private = NULL;
}

/** Find pending IRQ
    @param[in]     exc  An exception frame
    @param[in,out] irqp An IRQ number to be processed
 */
static int
gic_pl390_find_pending_irq(struct _exception_frame *exc __attribute__((unused)), irq_no *irqp) {
	int   rc;
	irq_no i;

	for( i = 0; GIC_INT_MAX > i; ++i) {
		if ( gicd_probe_pending(i) ) {

			rc = IRQ_FOUND;
			*irqp = i;
			goto found;
		}
	}

	rc = IRQ_NOT_FOUND ;
found:
	return rc;
}

/** Get current interrupt priority 
 */
uint32_t
aarch64_get_intr_priority(void){

	return *REG_GIC_GICC_PMR;
}

/** Set interrupt priority 
 */
void
aarch64_set_intr_priority(uint32_t prio){

	*REG_GIC_GICC_PMR = prio;
}

/** Raise Interrupt(Debugging use)
    @param[in] irq     IRQ number
 */
void
gic_raise_irq(irq_no irq){

	gicd_set_pending(irq);	
}

#if defined(INSTALL_TEST_HANDLER)
int 
test_handler(irq_no irq, struct _exception_frame *exc, void *private __attribute__((unused))){

	kprintf("Handler\n");
	return IRQ_HANDLED;
}
#endif  /*  INSTALL_TEST_HANDLER  */

/** Initialize interrupt facilities 
 */
void 
aarch64_init_interrupt(void){
	int i;

	aarch64_setup_vector();

	irq_register_pending_irq_finder(&arm_gic_find_irq);

	for(i = 0; GIC_INT_MAX > i; ++i) 
		irq_register_ctrlr(i, &arm_gic_ctrlr);

	gic_pl390_initialize(&arm_gic_ctrlr);
#if defined(INSTALL_TEST_HANDLER)
	irq_register_handler(35, IRQ_ATTR_EXCLUSIVE | IRQ_ATTR_NESTABLE | IRQ_ATTR_EDGE, 
	    1, NULL, test_handler);
#endif  /*  INSTALL_TEST_HANDLER  */
}
