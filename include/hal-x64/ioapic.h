/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014-2018 Takeharu KATO                                 */
/*                                                                    */
/*                                                                    */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_IOAPIC_H)
#define  _HAL_IOAPIC_H 

#define MAX_NR_IOAPICS		32
#define MAX_IOAPIC_IRQS		64

#if !defined(ASM_FILE)
#include <kern/freestanding.h>

typedef struct _io_apic {
	uint32_t           id;
	uintptr_t	paddr; /* where is it in phys space */
	void	       *kaddr; /* kernel mapped address */
	uint32_t         pins;
	uint32_t     gsi_base;
}io_apic;
#endif  /*  !ASM_FILE  */
#endif  /*  _HAL_IOAPIC_H   */
