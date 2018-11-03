/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014-2018 Takeharu KATO                                 */
/*                                                                    */
/*  ACPI                                                              */
/*                                                                    */
/*  This file is derived from MINIX acpi.c                            */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>
#include <hal/hal.h>

//#define DEBUG_ACPI_LAPIC
//#define DEBUG_ACPI_IOAPIC

#define ACPI_SDT_TRANS_TYPE_NONE  (0)
#define ACPI_SDT_TRANS_TYPE_FACP  (1)
#define ACPI_SDT_TRANS_TYPE_APIC  (2)
#define ACPI_SDT_TRANS_TYPE_HPET  (3)
#define ACPI_SDT_TRANS_TYPE_NR    (4)

typedef struct _acpi_sdt_trans{
	int         type;
	uint32_t   paddr;
	void      *kaddr;
	size_t	  length;
}acpi_sdt_trans;

typedef struct _acpi_info{
	acpi_rsdp                     rsdp;
	acpi_sdt_trans sdt_trans[MAX_RSDT];
	int                      sdt_count;
}acpi_info;

static acpi_info g_acpi_info;

static void
acpi_copy_phys(uintptr_t phys, void *target, size_t len){

	memcpy(target, (void *)PHY_TO_KERN_STRAIGHT(phys), len);
}

static int 
acpi_rsdp_test(acpi_rsdp *rsdp){
	uint8_t   total;
	int           i;

	for (i = 0, total = 0; ACPI_RSD_CHECKSUM_LEN > i; ++i)
		total += ((uint8_t *)rsdp)[i];

	total &= 0xff;

	if ( total != 0 ) 
		return 0;

	if (memcmp(rsdp->signature, ACPI_RSD_SIGNATURE_STRING, ACPI_RSD_SIGNATURE_LEN) != 0 ) 
		return 0;

	return 1;
}

static int 
get_acpi_rsdp(void){
	int          rc;
	uint16_t   ebda;
	uint32_t     sp;
	acpi_rsdp *rsdp;

	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_copy_phys(ACPI_EBDA_EBDAP_ADDR, &ebda, sizeof(ebda));
	if ( ebda != 0 ) {

		ebda <<= 4;  /*  RSDP signature is always on a 16 byte boundary. */
		for(sp = ebda; (ebda + ACPI_EBDA_LEN) > sp; sp += 16) {

			rsdp = (acpi_rsdp *)PHY_TO_KERN_STRAIGHT(sp);
			rc = acpi_rsdp_test(rsdp);
			if ( rc != 0 )
				goto found;
		}
	}

	/* Try to search RSDP in BIOS read only mem space */
	for(sp = ACPI_RSDP_BIOS_AREA_START; ACPI_RSDP_BIOS_AREA_END > sp; sp += 16) {

		rsdp = (acpi_rsdp *)PHY_TO_KERN_STRAIGHT(sp);
		rc = acpi_rsdp_test(rsdp);
		if ( rc != 0 )
			goto found;
	}

	return 0;	/* RSDP not found */

found:
	memcpy(&g_acpi_info.rsdp, rsdp, sizeof(acpi_rsdp));
	return 1;
}

static int 
acpi_check_csum(acpi_sdt_header *tb, size_t size){
	uint8_t total = 0;
	int             i;

	for (i = 0; i < size; i++)
		total += ((uint8_t *)tb)[i] & 0xff;

	return total == 0 ? 0 : -1;
}

static int
read_sdt_at(uintptr_t addr, acpi_sdt_header *tb, size_t size,  const char *name){
	int rc;

	acpi_copy_phys(addr, tb, sizeof(acpi_sdt_header));
	rc = memcmp(tb->signature, name, ACPI_SDT_SIGNATURE_LEN);
	if ( rc != 0 ) {

		kprintf("ERROR ACPI signature does not match with %s\n", name);
		return -1;
	}
	
	if (size < tb->length) {
		kprintf("ERROR ACPI buffer too small for %s\n", name);
		return -1;
	}

	acpi_copy_phys(addr, tb, size);
	rc = acpi_check_csum(tb, tb->length);
	if ( rc != 0 ) {

		kprintf("ERROR acpi %s checksum does not match\n", name);
		return -1;
	}

	return tb->length;
}

static void * 
madt_get_typed_item(acpi_madt_hdr * hdr, uint8_t type, unsigned int *idxp){
	void            *t, *end;
	acpi_madt_item_hdr *hdrp;
	int                    i;
	unsigned int         idx;

	kassert(idxp != NULL);
	idx = *idxp;

	t = (void *)hdr + sizeof(acpi_madt_hdr);
	end = (void *)hdr + hdr->hdr.length;

	for(i = 0; end > t; t += ((acpi_madt_item_hdr *)t)->length) {

		hdrp = (acpi_madt_item_hdr *)t;
		if ( type == hdrp->type ) {

			if ( i == idx ) {

				*idxp = idx;
				return t;
			} else
				++i;
		}

		if ( hdrp->length == 0 )
			break;
	}

	return NULL;
}

static acpi_madt_hdr *
acpi_get_table_base(int type){
	int i;

	for(i = 0; i < g_acpi_info.sdt_count; ++i) {

		if ( g_acpi_info.sdt_trans[i].type == type )
			return (acpi_madt_hdr *)g_acpi_info.sdt_trans[i].kaddr;
	}

	return NULL;
}

acpi_madt_lapic *
acpi_get_lapic(unsigned int *idxp){
	unsigned int         idx;
	acpi_madt_hdr  *madt_hdr;
	acpi_madt_lapic     *ret;

	kassert(idxp != NULL);

	idx = *idxp;

	if (idx == 0) {

		madt_hdr = (acpi_madt_hdr *)acpi_get_table_base(ACPI_SDT_TRANS_TYPE_APIC);
		if (madt_hdr == NULL)
			goto error_out;
	}

	ret = (acpi_madt_lapic *)madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_LAPIC, &idx);
	if ( ret == NULL )
		goto error_out;

	/* report only usable CPUs */
	if ( ret->flags & ACPI_MADT_LAPIC_FLAGS_PROC_ENABLED ) {

		*idxp = idx + 1;
		return ret;
	}

error_out:
	return NULL;
}

acpi_madt_ioapic *
acpi_get_ioapic(unsigned int *idxp){
	unsigned int         idx;
	acpi_madt_hdr  *madt_hdr;
	acpi_madt_ioapic    *ret;

	kassert(idxp != NULL);
	idx = *idxp;

	if (idx == 0) {

		madt_hdr = (acpi_madt_hdr *)acpi_get_table_base(ACPI_SDT_TRANS_TYPE_APIC);
		if (madt_hdr == NULL)
			goto error_out;
	}

	ret = (acpi_madt_ioapic *)madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, &idx);
	if ( ret == NULL )
		goto error_out;

	*idxp = idx + 1;

	return ret;

error_out:
	return NULL;
}

/** Initialize ACPI
 */
void
x64_acpi_init(void){
	int                 s, i;
	acpi_sdt_header      hdr;
        acpi_rsdt           rsdt;
#if defined(DEBUG_ACPI_LAPIC) || defined(DEBUG_ACPI_IOAPIC)
	unsigned int         idx;
#endif  /*  DEBUG_ACPI_LAPIC || DEBUG_ACPI_IOAPIC */
#if defined(DEBUG_ACPI_LAPIC)
	acpi_madt_lapic   *lapic;
#endif  /*  DEBUG_ACPI_LAPIC  */
#if defined(DEBUG_ACPI_IOAPIC)
	acpi_madt_ioapic *ioapic;
#endif  /*  DEBUG_ACPI_IOAPIC  */

	memset(&g_acpi_info, 0, sizeof(acpi_info));

	if ( get_acpi_rsdp() != 1 ) {

		kprintf("WARNING : Cannot configure ACPI\n");
		return;
	} 

	s = read_sdt_at(g_acpi_info.rsdp.rsdt_addr, (acpi_sdt_header *)&rsdt,
			sizeof(acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));
	g_acpi_info.sdt_count = (s - sizeof(acpi_sdt_header)) / sizeof(uint32_t);
	
	for (i = 0; g_acpi_info.sdt_count > i ; ++i) {

		acpi_copy_phys(rsdt.data[i], &hdr, sizeof(acpi_sdt_header));

		if ( memcmp(hdr.signature, "APIC", ACPI_SDT_SIGNATURE_LEN) == 0 ) 
			g_acpi_info.sdt_trans[i].type = ACPI_SDT_TRANS_TYPE_APIC;
		else if ( memcmp(hdr.signature, "FACP", ACPI_SDT_SIGNATURE_LEN) == 0 ) 
			g_acpi_info.sdt_trans[i].type = ACPI_SDT_TRANS_TYPE_FACP;
		else if ( memcmp(hdr.signature, "HPET", ACPI_SDT_SIGNATURE_LEN) == 0 ) 
			g_acpi_info.sdt_trans[i].type = ACPI_SDT_TRANS_TYPE_HPET;
		else
			g_acpi_info.sdt_trans[i].type = ACPI_SDT_TRANS_TYPE_NONE;

		g_acpi_info.sdt_trans[i].paddr = rsdt.data[i];
		g_acpi_info.sdt_trans[i].kaddr = (void *)PHY_TO_KERN_STRAIGHT(rsdt.data[i]);
		g_acpi_info.sdt_trans[i].length = hdr.length;
	}

#if defined(DEBUG_ACPI_LAPIC)
	for(idx = 0; ;) {

		lapic = acpi_get_lapic(&idx);
		if ( lapic == NULL )
			break;
		
		kprintf("ACPI APIC[%d] cpuid=%d apicid=%d flags=0x%x\n", 
			idx - 1, lapic->acpi_cpu_id, lapic->apic_id, lapic->flags);
	}
#endif  /*  DEBUG_ACPI_LAPIC  */

#if defined(DEBUG_ACPI_IOAPIC)
	for(idx = 0; ; ) {

		ioapic = acpi_get_ioapic(&idx);
		if ( ioapic == NULL )
			break;
		
		kprintf("ACPI IOAPIC[%d] id=%d address=0x%x global int base=0x%x\n", 
			idx - 1, ioapic->id, ioapic->address, ioapic->global_int_base);
	}
#endif  /*  DEBUG_ACPI_IOAPIC  */
}
