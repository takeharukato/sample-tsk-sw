/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  ACPI table definitions                                            */
/*  This file is derived from MINIX acpi.h                            */
/**********************************************************************/
#if !defined(_HAL_ACPI_H)
#define  _HAL_ACPI_H 

#include <kern/freestanding.h>

#define ACPI_EBDA_EBDAP_ADDR      (0x40E)   /*< Pointer for Extended BIOS Data Area(EBDA)  */
#define ACPI_EBDA_LEN             (0x400)   /*< Extended BIOS Data Area(EBDA) length ( 1 KiB )  */
#define ACPI_RSDP_BIOS_AREA_START (0xE0000) /*< RSDP in the main BIOS area below 1 MB */
#define ACPI_RSDP_BIOS_AREA_END   (0x100000)

/** Root System Description Pointer */
typedef struct _acpi_rsdp {
	char   signature[8];  /*< "RSD PTR " */
	uint8_t	   checksum;
	char	   oemid[6];
	uint8_t	   revision;
	uint32_t  rsdt_addr;
	uint32_t     length;
}acpi_rsdp; 

#define ACPI_RSD_SIGNATURE_STRING       "RSD PTR "
#define ACPI_RSD_SIGNATURE_LEN          (8)
#define ACPI_RSD_CHECKSUM_LEN     (20)

#define ACPI_SDT_SIGNATURE(name)	#name
#define ACPI_SDT_SIGNATURE_LEN	        (4)
#define MAX_RSDT	                (35) /* ACPI defines 35 signatures */

/* header common to all system description tables */
typedef struct _acpi_sdt_header {
	char	signature[ACPI_SDT_SIGNATURE_LEN];
	uint32_t                           length;
	uint8_t	                         revision;
	uint8_t	                         checksum;
	char	                         oemid[6];
	char                      oem_table_id[8];
	uint32_t                     oem_revision;
	uint32_t                       creator_id;
	uint32_t                 creator_revision;
}acpi_sdt_header;

typedef struct _acpi_rsdt {
	struct _acpi_sdt_header	hdr;
	uint32_t     data[MAX_RSDT];
}acpi_rsdt;

typedef struct _acpi_generic_address {
	uint8_t address_space_id;
	uint8_t register_bit_width;
	uint8_t register_bit_offset;
	uint8_t access_size;
	uint64_t address;
}acpi_generic_address;

typedef struct _acpi_fadt_header{
	struct _acpi_sdt_header                hdr;
	uint32_t                              facs;
	uint32_t                              dsdt;
	uint8_t                              model;
	uint8_t               preferred_pm_profile;
	uint16_t                           sci_int;
	uint32_t                           smi_cmd;
	uint8_t                        acpi_enable;
	uint8_t                       acpi_disable;
	uint8_t                         s4bios_req;
	uint8_t                         pstate_cnt;
	uint32_t                      pm1a_evt_blk;
	uint32_t                      pm1b_evt_blk;
	uint32_t                      pm1a_cnt_blk;
	uint32_t                      pm1b_cnt_blk;
	uint32_t                       pm2_cnt_blk;
	uint32_t                        pm_tmr_blk;
	uint32_t                          gpe0_blk;
	uint32_t                          gpe1_blk;
	uint8_t                        pm1_evt_len;
	uint8_t                        pm1_cnt_len;
	uint8_t                        pm2_cnt_len;
	uint8_t                         pm_tmr_len;
	uint8_t                       gpe0_blk_len;
	uint8_t                       gpe1_blk_len;
	uint8_t                          gpe1_base;
	uint8_t                            cst_cnt;
	uint16_t                        p_lvl2_lat;
	uint16_t                        p_lvl3_lat;
	uint16_t                        flush_size;
	uint16_t                      flush_stride;
	uint8_t                        duty_offset;
	uint8_t                         duty_width;
	uint8_t                           day_alrm;
	uint8_t                           mon_alrm;
	uint8_t                            century;
	uint16_t                    iapc_boot_arch;
	uint8_t                          reserved1;
	uint32_t                             flags;
	struct _acpi_generic_address     reset_reg;
	uint8_t                        reset_value;
	uint8_t                       reserved2[3];
	uint64_t                             xfacs;
	uint64_t                             xdsdt;
	struct _acpi_generic_address xpm1a_evt_blk;
	struct _acpi_generic_address xpm1b_evt_blk;
	struct _acpi_generic_address xpm1a_cnt_blk;
	struct _acpi_generic_address xpm1b_cnt_blk;
	struct _acpi_generic_address  xpm2_cnt_blk;
	struct _acpi_generic_address   xpm_tmr_blk;
	struct _acpi_generic_address     xgpe0_blk;
	struct _acpi_generic_address     xgpe1_blk;
}acpi_fadt_header;

typedef struct _acpi_madt_hdr {
	struct _acpi_sdt_header	        hdr;
	uint32_t	 local_apic_address;
	uint32_t	              flags;
}acpi_madt_hdr;

#define ACPI_MADT_TYPE_LAPIC		(0)
#define ACPI_MADT_TYPE_IOAPIC		(1)
#define ACPI_MADT_TYPE_INT_SRC		(2)
#define ACPI_MADT_TYPE_NMI_SRC		(3)
#define ACPI_MADT_TYPE_LAPIC_NMI	(4)
#define ACPI_MADT_TYPE_LAPIC_ADRESS	(5)
#define ACPI_MADT_TYPE_IOSAPIC		(6)
#define ACPI_MADT_TYPE_LSAPIC		(7)
#define ACPI_MADT_TYPE_PLATFORM_INT_SRC	(8)
#define ACPI_MADT_TYPE_Lx2APIC		(9)
#define ACPI_MADT_TYPE_Lx2APIC_NMI	(10)

typedef struct _acpi_madt_item_hdr{
	uint8_t	  type;
	uint8_t	length;
}acpi_madt_item_hdr;

typedef struct _acpi_madt_lapic {
	struct _acpi_madt_item_hdr hdr;
	uint8_t	           acpi_cpu_id;
	uint8_t	               apic_id;
	uint32_t	         flags;
}acpi_madt_lapic;

#define ACPI_MADT_LAPIC_FLAGS_PROC_ENABLED (0x1)  /*< Processor Enabled */

typedef struct _acpi_madt_ioapic {
	struct _acpi_madt_item_hdr hdr;
	uint8_t	                    id;
	uint8_t	            __reserved;
	uint32_t               address;
	uint32_t	global_int_base;
}acpi_madt_ioapic;

typedef struct _acpi_madt_int_src {
	struct _acpi_madt_item_hdr hdr;
	uint8_t	                   bus;
	uint8_t	               bus_int;
	uint32_t	    global_int;
	uint16_t	     mps_flags;
}acpi_madt_int_src;

typedef struct _acpi_madt_nmi{
	struct _acpi_madt_item_hdr hdr;
	uint16_t	         flags;
	uint32_t	    global_int;
}acpi_madt_nmi;

void x64_acpi_init(void);

/* 
 * Returns a pointer to the io acpi structure in the MADT table in ACPI. The
 * pointer is valid only until paging is turned off. No memory is allocated in
 * this function thus no memory needs to be freed
 */
struct _acpi_madt_ioapic *acpi_get_ioapic(unsigned int *_idxp);
/* same as above for local APICs */
struct _acpi_madt_lapic *acpi_get_lapic(unsigned int *_idxp);
#endif  /*  _HAL_ACPI_H   */
