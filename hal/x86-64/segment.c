/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  GDT/IDT in Intel chips                                            */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"
#include "hal/segments.h"

static segment_desc_t gdt[GDT_NR];
static gate_descriptor_t idt[IDT_NR];
static descriptor_tbl_t gdt_tbl;
static descriptor_tbl_t idt_tbl;


static inline void
lidt(descriptor_tbl_t *addr){
        __asm __volatile("lidt (%0)" : : "r" (addr));
}

static inline void
set_mem_descriptor(segment_desc_t *desc, uint32_t addr, int type, int pl, int db) {
	
	desc->limit_low  = (addr & 0xffff);
	desc->limit_high = (addr >> 24) & 0xf;
	desc->base_low = 0x0;
	desc->base_high = 0x0;
	desc->type = type;
	desc->dpl = pl;
	desc->p = 1;
	desc->long_mode = 1;
	desc->db = db;
	desc->g = 1;
}

static inline void
set_tss_descriptor(segment_desc_t *desc) {
	uint32_t addr = sizeof(x86_64_tss_t) * 8192 - 1; /* 8192 is IO bitmap pages  */
	
	desc->limit_low  = (addr & 0xffff);
	desc->limit_high = (addr >> 24) & 0xf;
	desc->base_low = 0x0;
	desc->base_high = 0x0;
	desc->type = SD_T_TSS;
	desc->dpl = PL_KERN;
	desc->p = 1;
	desc->long_mode = 1;
	desc->db = 0;
	desc->g = 1;
}
void
setidt(int idx, void (*func)(void *),int typ, int dpl) {
	gate_descriptor_t *ip;

	ip = idt + idx;
	ip->gd_looffset = (unsigned long)func;
	ip->gd_selector = (KERN_CS<<3)|PL_KERN;
	ip->gd_ist = 0;
	ip->gd_xx = 0;
	ip->gd_type = typ;
	ip->gd_dpl = dpl;
	ip->gd_p = 1;
	ip->gd_hioffset = ((unsigned long)func)>>16 ;
}

void 
init_descriptor(void){
	int i;

	memset(&gdt[0], 0, sizeof(segment_desc_t) * GDT_NR);
	/*
	 *TODO: 第5引数(db)をマクロ化すること
	 */
	set_mem_descriptor(&gdt[KERN_CS], 0xfffff, SD_T_MRE, PL_KERN, 0);
	set_mem_descriptor(&gdt[KERN_DS], 0xfffff, SD_T_MRE, PL_KERN, 0);
	set_mem_descriptor(&gdt[USER_CS], 0xfffff, SD_T_MRE, PL_USER, 0);
	set_mem_descriptor(&gdt[USER_DS], 0xfffff, SD_T_MRE, PL_USER, 1);
	set_tss_descriptor(&gdt[GPROC0_SEL]);
	/*
	 *TODO:TSSへの参照を設定する
	 */
	
	/*
	 * GDT登録
	 */
	gdt_tbl.dt_limit = GDT_NR * sizeof(gdt[0]) - 1;
	gdt_tbl.dt_base = (unsigned long)&gdt[0];
	lgdt(&gdt_tbl);
#if 0
	for(i = 0; i < IDR_NR; ++i)
		setidt(x, &IDTVEC(rsvd), SDT_SYSIGT, SEL_KPL, 0);

	setidt(0, &IDTVEC(div),  SDT_SYSIGT, SEL_KPL);
	setidt(1, &IDTVEC(dbg),  SDT_SYSIGT, SEL_KPL);
	setidt(2, &IDTVEC(nmi),  SDT_SYSIGT, SEL_KPL);
 	setidt(3, &IDTVEC(bpt),  SDT_SYSIGT, SEL_UPL);
	setidt(4, &IDTVEC(ofl),  SDT_SYSIGT, SEL_KPL);
	setidt(5, &IDTVEC(bnd),  SDT_SYSIGT, SEL_KPL);
	setidt(6, &IDTVEC(ill),  SDT_SYSIGT, SEL_KPL);
	setidt(7, &IDTVEC(dna),  SDT_SYSIGT, SEL_KPL);
	setidt(8, &IDTVEC(dblfault), SDT_SYSIGT, SEL_KPL);
	setidt(9, &IDTVEC(fpusegm),  SDT_SYSIGT, SEL_KPL);
	setidt(10, &IDTVEC(tss),  SDT_SYSIGT, SEL_KPL);
	setidt(11, &IDTVEC(missing),  SDT_SYSIGT, SEL_KPL);
	setidt(12, &IDTVEC(stk),  SDT_SYSIGT, SEL_KPL);
	setidt(13, &IDTVEC(prot),  SDT_SYSIGT, SEL_KPL);
	setidt(14, &IDTVEC(page),  SDT_SYSIGT, SEL_KPL);
	setidt(16, &IDTVEC(fpu),  SDT_SYSIGT, SEL_KPL);
	setidt(17, &IDTVEC(align), SDT_SYSIGT, SEL_KPL);
	setidt(18, &IDTVEC(mchk),  SDT_SYSIGT, SEL_KPL);
	setidt(19, &IDTVEC(xmm), SDT_SYSIGT, SEL_KPL);

	idt_tbl.rd_limit = sizeof(idt[0]) - 1;
	idt_tbl.rd_base = (long) idt;
	lidt(&idt_tbl);

#endif
	
}
