/* -*- mode: asm; coding:utf-8 -*- */
/* data.S - data for bootstrap */
/* Copyright (C) 1999  Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#define ASM_FILE   1
#include <kern/param.h>

#include <hal/multiboot2.h>
#include <hal/segments.h>


.section .boot_data, "aw", @progbits

.global boot_entry_addr	
.global	pre_gdt_p
.global	pre_pml4
.global	pre_pdpt_low
.global	pre_pdpt_high
.global	pre_pd
.global kmap_pml4
.global	gdt
.global	idt
.global	idt_ptr

.extern boot_main
.extern	bsp_stack
boot_entry_addr:
	.quad boot_main

.align 0x10
idt:
	#.fill 256, 0x10, 0x0  idt_t = 16(0x10)byte x 256 entries
	.fill 512, 0x8,	0x0
idt_ptr:
	.word	(. - idt - 1)
	.quad	idt


.p2align 3
pre_gdt:
	GDT_NULL_ENTRY
	GDT_NULL_ENTRY
	SET_GDT_ENTRY( GDT_SEG_32, GDT_KERNEL, GDT_CS, 0x0, 0xFFFFFFFF)
	SET_GDT_ENTRY( GDT_SEG_32, GDT_KERNEL, GDT_DS, 0x0, 0xFFFFFFFF)
	SET_GDT_ENTRY( GDT_SEG_64, GDT_KERNEL, GDT_CS, 0x0, 0xFFFFFFFF)
	SET_GDT_ENTRY( GDT_SEG_64, GDT_KERNEL, GDT_DS, 0x0, 0xFFFFFFFF)
	SET_GDT_ENTRY( GDT_SEG_64, GDT_USER, GDT_CS, 0x0, 0xFFFFFFFF)
	SET_GDT_ENTRY( GDT_SEG_64, GDT_USER, GDT_DS, 0x0, 0xFFFFFFFF)
pre_gdt_end:

pre_gdt_p:
	.word	pre_gdt_end - pre_gdt + 1
	.quad	pre_gdt


.p2align 12
pre_pml4:
    .zero   PAGE_SIZE
.p2align 12
pre_pdpt_low:
    .zero   PAGE_SIZE
.p2align 12
pre_pdpt_high:
    .zero   PAGE_SIZE
.p2align 12
pre_pd:
    .zero   PAGE_SIZE

.p2align 12
kmap_pml4:
    .zero   PAGE_SIZE

.p2align 12
gdt:
    .zero   PAGE_SIZE

