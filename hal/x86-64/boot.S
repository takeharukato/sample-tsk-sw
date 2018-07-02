/* boot.S - bootstrap the kernel */
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

#define ASM	1
#include "hal/multiboot.h"

#define LOAD_ADDRESS 0x100000 // 1MB
#define PGTABLE_BASE 0x8000
	
.code32
.text

	.globl	start, _start, _edata

	/* Align 32 bits boundary.  */
	.align	4
start:
_start:
	jmp		multiboot_entry
	/* Multiboot header.  */
.align	4
multiboot_header:
	/* magic */
	.long	MULTIBOOT_HEADER_MAGIC
	/* flags */
	.long	MULTIBOOT_HEADER_FLAGS
	/* checksum */
	.long	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
	/* header_addr */
	.long	multiboot_header
	/* load_addr */
	.long	_start
	/* load_end_addr */
	.long	_edata
	/* bss_end_addr */
	.long	_end
	/* entry_addr */
	.long	multiboot_entry

	/* This entry is not used actually.  */
multiboot_entry:
	/* Initialize the stack pointer.  */
	movl	$stack32_end, %esp
	/* Reset EFLAGS.  */
	pushl	$0
	popf

	/* Push the pointer to the Multiboot information structure.  */
	pushl	%ebx
	/* Push the magic value.  */
	pushl	%eax
	movl	%ebx, __boot_structure
	movl	%eax, __boot_magic	

	// zero out 3 pages of ram where we're going to build some page tables
	xor     %eax, %eax
	mov     $PGTABLE_BASE, %edi
	mov     $0x3000, %ecx
	rep     stosb

	// top level page table
	mov     $((PGTABLE_BASE + 0x1000) | 0x3), %eax
	mov     %eax, PGTABLE_BASE
	mov     $((PGTABLE_BASE + 0x2000) | 0x3), %eax
	mov     %eax, (PGTABLE_BASE + 0x1000)

	// 2MB pages to identity map the first 32MB ram
	mov     $(0 | (1<<7) | 3), %eax
	mov     %eax, (PGTABLE_BASE + 0x2000)
	add     $0x200000, %eax
	mov     %eax, (PGTABLE_BASE + 0x2008)
	add     $0x200000, %eax        
	mov     %eax, (PGTABLE_BASE + 0x2010)
	add     $0x200000, %eax        
	mov     %eax, (PGTABLE_BASE + 0x2018)
	add     $0x200000, %eax        
	mov     %eax, (PGTABLE_BASE + 0x2020)
	add     $0x200000, %eax        
	mov     %eax, (PGTABLE_BASE + 0x2028)
	add     $0x200000, %eax        
	mov     %eax, (PGTABLE_BASE + 0x2030)
	add     $0x200000, %eax        
	mov     %eax, (PGTABLE_BASE + 0x2038)
	/* 16MB - 32MB */
	add     $0x200000, %eax        
	mov     %eax, (PGTABLE_BASE + 0x2040)
	add     $0x200000, %eax        
	mov     %eax, (PGTABLE_BASE + 0x2048)
	add     $0x200000, %eax        
	mov     %eax, (PGTABLE_BASE + 0x2050)
	add     $0x200000, %eax        
	mov     %eax, (PGTABLE_BASE + 0x2058)
	add     $0x200000, %eax        
	mov     %eax, (PGTABLE_BASE + 0x2060)
	add     $0x200000, %eax        
	mov     %eax, (PGTABLE_BASE + 0x2078)
	add     $0x200000, %eax        
	mov     %eax, (PGTABLE_BASE + 0x2080)
	add     $0x200000, %eax        
	mov     %eax, (PGTABLE_BASE + 0x2088)
	
	// set the page table base
	mov     $PGTABLE_BASE, %eax
	mov     %eax, %cr3

	// load the 64bit gdt
	lgdt	gdt64

	// set CR4.PAE
	mov     %cr4, %eax          
	bts     $5, %eax
	mov     %eax, %cr4        

	// enable long mode      
	// EFER.LME=1            
	mov     $0xc0000080, %ecx
	rdmsr   
	bts     $8, %eax             
	wrmsr  

	// enable paging, which will really enable long mode (CR0.PG)
	mov     %cr0, %eax
	bts     $31, %eax
	mov     %eax, %cr0

	ljmp	$0x08,$(long_64_mode)

.code64
long_64_mode:
	mov		$bsp_stack_end, %rsp

	movq	__boot_structure, %rsi
	movq	__boot_magic, %rdi
	callq   boot_main
loop:	hlt
	jmp	loop

gdt64:
	// first entry, also the gdt descriptor
	.word 0xffff
	.long gdt64
	.word 0
	// entry 64bit code segment
    .long 0x00000000  // base & limit are ignored
    .long 0x00af9a00  // type: 64 bit, code, <present>, priviledge 0
	
.data
.align 4
stack32:
	.skip 0x4000
stack32_end:
	
.align 8
__boot_structure:
	.skip 0x8
__boot_magic:
	.skip 0x8
__multiboot_end:
