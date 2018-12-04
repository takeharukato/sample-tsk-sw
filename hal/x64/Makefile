top=../..
include ${top}/Makefile.inc
UNAME := $(shell uname)
QEMU ?= qemu-system-x86_64
QEMU_KERNEL ?= ${top}/kernel.elf
QEMU_OPT ?= -m ${CONFIG_HAL_MEMORY_SIZE_MB} -nographic -serial mon:stdio
QEMU_DBG_OPT := -S -gdb tcp::1234
ifeq (${UNAME}, FreeBSD)
GRUB2_RESCUE=grub-mkrescue
GRUB2_RESCUE_OPT=
else
GRUB2_RESCUE=grub2-mkrescue
GRUB2_RESCUE_OPT=-d /usr/lib/grub/i386-pc
endif

CFLAGS += -I${top}/include
ASM_OFFSET_CFLAGS = $(shell echo ${CFLAGS}|sed -e 's@-ggdb[0-9]*@@g')
objects = thr_context.o ctxsw_glue.o fsimg.o
ifeq ($(CONFIG_HAL),y)
objects += boot.o boot64.o data.o early_console.o kmap.o vector.o segments.o \
	 idt.o lgdtr.o ltr.o lidtr.o traps.o exception.o i8259.o timer.o console.o acpi.o \
	lapic.o
endif

lib=libhal.a

all: ${top}/include/hal/asm-offset.h kernel.lds ${lib}

${lib}:${objects}
	${AR} ${ARFLAGS} $@ $^
	${RANLIB} $@

asm-offset.s: asm-offset.c
	${CC} ${ASM_OFFSET_CFLAGS} -S -o $@ $<

gen-asm-offset: asm-offset.s ${top}/tools/asmoffset/gen-asm-offset.py
	${top}/tools/asmoffset/gen-asm-offset.py \
	-i ${top}/hal/hal/asm-offset.s \
	-o ${top}/include/hal/asm-offset.h

vector.o: vector.S

vector.S: ${top}/tools/x64/gen-vec.py
	${RM} vector.S
	${top}/tools/x64/gen-vec.py > $@

${top}/include/hal/asm-offset.h: gen-asm-offset

fsimg.S: fsimg.S.in ${FSIMG_FILE}
	${RM} $@
	${SED} -e 's|__fsimg_file__|${FSIMG_FILE}|g' 	\
	       -e 's|__PAGE_SIZE__|${CONFIG_HAL_PAGE_SIZE}|g' 		\
	fsimg.S.in > $@

kernel.lds: kernel.lds.in ${top}/.config
	${RM} $@
	${SED} -e 's|__KHEAP_SIZE__|${CONFIG_HEAP_SIZE}*1024*1024|g' 	\
	       -e 's|__PAGE_SIZE__|${CONFIG_HAL_PAGE_SIZE}|g' 		\
	       -e 's|__TSTACK_PAGES__|${CONFIG_TSTACK_PAGE_NR}|g'	\
	       -e 's|__ISTACK_PAGES__|${CONFIG_ISTACK_PAGE_NR}|g'	\
	       -e 's|__KERN_VMA_BASE__|${CONFIG_HAL_KERN_VMA_BASE}|g'	\
	$< > $@

${top}/kernel.iso: ${QEMU_KERNEL}
	mkdir -p ${top}/rfs/boot/grub
	mkdir -p ${top}/rfs/boot/modules
	cp ${top}/hal/${CPU}/grub.cfg ${top}/rfs/boot/grub
	cp ${top}/kernel.elf ${top}/rfs/boot
	${GRUB2_RESCUE} ${GRUB2_RESCUE_OPT} -o ${top}/kernel.iso ${top}/rfs
	if [ -d ${top}/rfs ]; then     					\
	  	rm -fr ${top}/rfs;					\
	fi;

run: ${top}/kernel.iso
	${QEMU} ${QEMU_OPT} -cdrom $<

run-debug: ${top}/kernel.iso
	${QEMU} ${QEMU_OPT} ${QEMU_DBG_OPT} -cdrom $<

clean:
	${RM} *.o ${lib} vector.S asm-offset.s ${top}/include/hal/asm-offset.h

distclean:clean
	${RM} \#* *~
