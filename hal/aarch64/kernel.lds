ENTRY(_start)
SECTIONS
{
	. = 0x40000000;
	.text : { *(.text) }
	.data : { *(.data) }
	. = ALIGN(4096);
        __bss_start = .;
	.bss : {
 	. = ALIGN(4096 * 1);
	bsp_stack = .;
	. += (4096 * 1);
	bsp_stack_end = .;
	*(.bss COMMON) }
	. = ALIGN(4096);
        __bss_end = .;
	_edata = .;			/* End of data section */
	. = ALIGN(4096);
	_payload_start = .;
	.payload : { *(.payload) }
	_payload_end = .;
	. = ALIGN(4096);
	_end = . ;
	_kend = .;
	_kheap_start = .;
	. += (2*1024*1024);
	_kheap_end = .;
}

