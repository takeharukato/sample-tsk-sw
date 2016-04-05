/* ld script to make 32bit boot loader
 * This is derived from Martin Mares's ld script for i386 Linux kernel.
 * Copyright Takeharu KATO 2000 
 */
OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

ENTRY(_start)
SECTIONS
{
	. = 0x100000;

	/* text/read-only data */
	.text :	{ *(.text .gnu.linkonce.t.*) }

	.rodata : { *(.rodata .rodata.*) }

	/* writable data  */
	.data : { *(.data .gnu.linkonce.d.* .got*) }
	  __bss_start = .;		/* BSS */
	    .bss : {
	    	*(.bss)
	}
	  _end = . ;
	 _edata = .;			/* End of data section */
	/* payload */
	. = ALIGN(0x1000);
	_payload_start = .;
	.payload : { *(.payload) }
	_payload_end = .;

	_end = . ;
  _kend = .;
  /* Sections to be discarded */
  /DISCARD/ : {
	*(.text.exit)
	*(.data.exit)
	*(.exitcall.exit)
	}

  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }

}

