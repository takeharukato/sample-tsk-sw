/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014-2018 Takeharu KATO                                 */
/*                                                                    */
/*  Local APIC relevant definitions                                   */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_LAPIC_H)
#define  _HAL_LAPIC_H 

#define LAPIC_DEFAULT_ADDR  (0xfee00000) /* default local apic address */

#define LAPIC_ID	(0x020)
#define LAPIC_VERSION	(0x030)
#define LAPIC_TPR	(0x080)
#define LAPIC_EOI	(0x0b0)
#define LAPIC_LDR	(0x0d0)
#define LAPIC_DFR	(0x0e0)
#define LAPIC_SIVR	(0x0f0)
#define LAPIC_ISR	(0x100)
#define LAPIC_TMR	(0x180)
#define LAPIC_IRR	(0x200)
#define LAPIC_ESR	(0x280)
#define LAPIC_ICR1	(0x300)
#define LAPIC_ICR2	(0x310)
#define LAPIC_LVTTR	(0x320)
#define LAPIC_LVTTMR	(0x330)
#define LAPIC_LVTPCR	(0x340)
#define LAPIC_LINT0	(0x350)
#define LAPIC_LINT1	(0x360)
#define LAPIC_LVTER	(0x370)
#define LAPIC_TIMER_ICR	(0x380)
#define LAPIC_TIMER_CCR	(0x390)
#define LAPIC_TIMER_DCR	(0x3e0)

#define APIC_TDCR_2	0x00
#define APIC_TDCR_4	0x01
#define APIC_TDCR_8	0x02
#define APIC_TDCR_16	0x03
#define APIC_TDCR_32	0x08
#define APIC_TDCR_64	0x09
#define APIC_TDCR_128	0x0a
#define APIC_TDCR_1	0x0b

#define LAPIC_LVTT_VECTOR_MASK	(0x000000FF)
#define LAPIC_LVTT_DS_PENDING	(1 << 12)
#define LAPIC_LVTT_MASK		(1 << 16)
#define LAPIC_LVTT_TM		(1 << 17)

#define LAPIC_LVT_IIPP_MASK	(0x00002000)
#define LAPIC_LVT_IIPP_AH	(0x00002000)
#define LAPIC_LVT_IIPP_AL	(0x00000000)

#define LAPIC_ENABLE		        (0x100)
#define LAPIC_FOCUS_DISABLED            (1 << 9)
#define LAPIC_SIV                       (0xff)

#define LAPIC_ICR_DM_MASK		(0x00000700)
#define LAPIC_ICR_VECTOR		(LAPIC_LVTT_VECTOR_MASK)
#define LAPIC_ICR_DM_FIXED		(0 << 8)
#define LAPIC_ICR_DM_LOWEST_PRIORITY	(1 << 8)
#define LAPIC_ICR_DM_SMI		(2 << 8)
#define LAPIC_ICR_DM_RESERVED		(3 << 8)
#define LAPIC_ICR_DM_NMI		(4 << 8)
#define LAPIC_ICR_DM_INIT		(5 << 8)
#define LAPIC_ICR_DM_STARTUP		(6 << 8)
#define LAPIC_ICR_DM_EXTINT		(7 << 8)

#define LAPIC_ICR_DM_PHYSICAL		(0 << 11)
#define LAPIC_ICR_DM_LOGICAL		(1 << 11)

#define LAPIC_ICR_DELIVERY_PENDING	(1 << 12)

#define LAPIC_ICR_INT_POLARITY		(1 << 13)

#define LAPIC_ICR_LEVEL_ASSERT		(1 << 14)
#define LAPIC_ICR_LEVEL_DEASSERT	(0 << 14)

#define LAPIC_ICR_TRIGGER		(1 << 15)

#define LAPIC_ICR_INT_MASK		(1 << 16)

#define LAPIC_ICR_DEST_FIELD		(0 << 18)
#define LAPIC_ICR_DEST_SELF		(1 << 18)
#define LAPIC_ICR_DEST_ALL		(2 << 18)
#define LAPIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#define LAPIC_TIMER_INT_VECTOR		(0xf0)
#define LAPIC_ERROR_INT_VECTOR		(0xfe)
#define LAPIC_SPURIOUS_INT_VECTOR	(0xff)
#define LAPIC_INT_VECTOR_MIN            LAPIC_TIMER_INT_VECTOR
#define LAPIC_INT_VECTOR_MAX            LAPIC_SPURIOUS_INT_VECTOR

#if !defined(ASM_FILE)
#include <kern/freestanding.h>

#define LAPIC_ADDR_MASK     (~(0x100000ULL - 1) )

typedef struct _lapic{
	int32_t      id;
	void     *kaddr;
	void  *eoi_addr;
	uintptr_t paddr;
}lapic;

int x64_lapic_enable(uint32_t cpu);
void x64_lapic_disable(void);
void x64_lapic_set_timer_periodic(const uint32_t freq_us);
void x64_lapic_set_timer_one_shot(const uint32_t usec);
void x64_lapic_stop_timer(void);
void x64_lapic_restart_timer(void);
int x64_lapic_init(void);
#endif  /*  !ASM_FILE  */
#endif  /*  _HAL_LAPIC_H   */
