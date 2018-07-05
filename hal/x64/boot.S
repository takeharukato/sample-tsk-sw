/* -*- mode: asm; coding:utf-8 -*- */
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

#define ASM_FILE
#include <kern/param.h>

#include <hal/asm-offset.h>
#include <hal/multiboot2.h>
#include <hal/thread_info.h>
#include <hal/boot64.h>
#include <hal/x64.h>
#include <hal/segments.h>
#include <hal/pgtbl.h>
#include <hal/kmap.h>

#define  MBH_MAGIC                              MULTIBOOT2_HEADER_MAGIC
#define  MBH_ARCH                               MULTIBOOT_ARCHITECTURE_I386
#define  MBH_LENGTH                             (mbhdr_end - mbhdr)
#define  MBH_CHECKSUM                           -(MBH_MAGIC + MBH_ARCH + MBH_LENGTH)

.section .boot_text, "ax"
.code32

.global mbhdr
.align MULTIBOOT_INFO_ALIGN
mbhdr:
		# Basic
		.long           MBH_MAGIC
		.long           MBH_ARCH
		.long           MBH_LENGTH
		.long           MBH_CHECKSUM

		# End tag
		.word           0,0
		.long           0x8
mbhdr_end:

.global start
.extern bsp_stack
.extern pre_gdt_p
.extern _phys_bss_start
.extern _phys_bss_end

start:
		cld
		cli
		# Store GRUB data
		movl    %eax, %edx
		movl    %ebx, %esi

		xorl    %eax, %eax
clear_bsp_stack:
	        movl    $_bsp_stack_start, %edi
	        movl    $_bsp_stack_end , %ecx
	        subl    %edi, %ecx      /* %rcx = __bsp_stack_end(%rcx) -
						__bsp_stack_start(%rdi) */
	        shrl    $2, %ecx        /* %rcx = %rcx / 4  */
	        rep     stosl
	
clear_bss:
	        movl    $_phys_bss_start, %edi
	        movl    $_phys_bss_end , %ecx
	        subl    %edi, %ecx      /* %rcx = __bss_end(%rcx) - __bss_start(%rdi) */
	        shrl    $2, %ecx        /* %rcx = %rcx / 4  */
	        rep     stosl

	        # Store Grub data to %edi
		movl    %edx, %edi
setup_bsp_stack:	
		# Setup Stack
		movl    $(bsp_stack + STACK_SIZE - TI_SIZE), %esp
		# Set thread info magic, cpu number
		movl    $THR_THREAD_INFO_MAGIC, TI_MAGIC_OFFSET(%esp)
		movl    $0x0, TI_PREEMPT_OFFSET(%esp)
	
        	# Setup pre-GDT
		lgdt    pre_gdt_p
		ljmp    $GDT_KERN_CODE32, $1f
1:
        	# Setup data segment selectors
		mov     $GDT_KERN_DATA32, %eax
		mov     %ax, %ds
		mov     %ax, %es
		mov     %ax, %ss
		xorl    %eax, %eax
		mov     %ax, %fs
		mov     %ax, %gs

		# Init FPU
		fninit

.extern pre_pml4
.extern pre_pdpt_low
.extern pre_pdpt_high
.extern pre_pd

	        movl    $(pre_pdpt_low), %eax
	        orl     $PAGE_PRESENT, %eax
	        movl    %eax, pre_pml4

	        movl    $(pre_pdpt_high), %eax
	        orl     $PAGE_PRESENT, %eax
	        movl    %eax, pre_pml4 + (PGTBL_ENTRY_SIZE * PML4_INDEX(KERN_VMA_BASE))

	        movl    $(pre_pd), %eax
		orl     $PAGE_PRESENT, %eax
	        movl    %eax, pre_pdpt_low  + (PGTBL_ENTRY_SIZE * PDPT_INDEX(KERN_PHY_BASE))
	        movl    %eax, pre_pdpt_high + (PGTBL_ENTRY_SIZE * PDPT_INDEX(KERN_VMA_BASE))

	        xorl    %eax, %eax    # physical_address 0x0 ...
	        orl     $PAGE_2MB, %eax
	        orl     $PAGE_WRITABLE, %eax
	        orl     $PAGE_PRESENT, %eax
	        movl    %eax, pre_pd + (PGTBL_ENTRY_SIZE * PD_INDEX(KERN_PHY_BASE))
	        movl    %eax, pre_pd + (PGTBL_ENTRY_SIZE * PD_INDEX(KERN_VMA_BASE))

		# Setup long mode page table
	        movl $(pre_pml4), %eax
	        movl %eax, %cr3

		#enable PAE and PSE and OSFXSR and OSXMMEXCPT 
	        movl    %cr4, %eax              # read Control register 4
	        orl     $CR4_PAE, %eax          # enable PAE
	        orl     $CR4_PSE, %eax          # enable PSE
		orl	$CR4_OS_FXSR, %eax      # enable OSFXSR
		orl	$CR4_OS_XMMEXCEPT, %eax # enable OSXMMEXCPT 
	        movl    %eax, %cr4              # re-write

        	#enter long mode
	        movl    $EFER, %ecx
	        rdmsr   # read EFER
	        bts     $8, %eax # enable long mode 
	        wrmsr   # rewrite

	        #enable paging, disable FPU Emulation and enable monitor FPU
		movl    $CR0_FPU_EMULATION, %ecx 
		notl	%ecx
		movl    %cr0, %eax
		andl	%ecx, %eax
	        orl     $CR0_PAGING, %eax
	        orl     $CR0_MONITOR_FPU, %eax
	        movl    %eax, %cr0

	        ljmp    $GDT_KERN_CODE64, $.entry_long

.code64
.extern boot_entry_addr
.entry_long:
	        # Setup Stack (Higher)
	        movq    $(KERN_VMA_BASE), %rax
	        addq    %rax, %rsp

	        # Setup data segment selectors
	        mov     $GDT_KERN_DATA64, %eax
	        mov     %ax, %ds
	        mov     %ax, %es
	        mov     %ax, %ss

	        # Just in Case
	        pushq   %rsi
	        pushq   %rdi
	        movq    $(KERN_VMA_BASE), %rax
	        addq    %rax, %rsi
		movq	(boot_entry_addr), %rax
		call    *%rax
loop:
	        hlt
	        jmp loop