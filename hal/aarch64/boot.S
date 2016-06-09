.global _start
start:	
_start:	
    ldr x30, =stack_end	// setup stack
    mov sp, x30
    bl boot_main
    b .

.data
.align 8
stack:
	.skip 0x2000
stack_end:
