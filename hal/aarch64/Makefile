top=../..
include ${top}/Makefile.inc

QEMU ?= qemu-system-aarch64
QEMU_KERNEL ?= ${top}/kernel.elf
QEMU_OPT ?= -M virt -cpu cortex-a57 -m ${CONFIG_HAL_MEMORY_SIZE_MB} -nographic -serial mon:stdio #-d trace:pl011_irq_state,trace:pl011_can_receive
QEMU_DBG_OPT := -S -gdb tcp::1234

CFLAGS += -I${top}/include
ASM_OFFSET_CFLAGS = $(shell echo ${CFLAGS}|sed -e 's@-ggdb[0-9]*@@g')
objects = early_console.o thr_context.o ctxsw.o fsimg.o
ifeq ($(CONFIG_HAL),y)
objects += boot.o boot64.o kmap.o data.o exception.o vector.o gic-pl390.o timer.o console.o
endif

lib=libhal.a

all:${top}/include/hal/asm-offset.h kernel.lds ${lib}

${lib}:${objects}
	${AR} ${ARFLAGS} $@ $^
	${RANLIB} $@

${top}/include/hal/asm-offset.h: gen-asm-offset

fsimg.S: fsimg.S.in ${FSIMG_FILE} ${top}/.config
	${RM} $@
	${SED} -e 's|__fsimg_file__|${FSIMG_FILE}|g' 	\
       -e 's|__PAGE_SIZE__|${CONFIG_HAL_PAGE_SIZE}|g' 	\
	fsimg.S.in > $@

asm-offset.s: asm-offset.c
	${CC} ${ASM_OFFSET_CFLAGS} -S -o $@ $<

gen-asm-offset: asm-offset.s ${top}/tools/asmoffset/gen-asm-offset.py
	${top}/tools/asmoffset/gen-asm-offset.py \
	-i ${top}/hal/hal/asm-offset.s \
	-o ${top}/include/hal/asm-offset.h

kernel.lds: kernel.lds.in ${top}/.config
	${RM} $@
	${SED} -e 's|__KHEAP_SIZE__|${CONFIG_HEAP_SIZE}*1024*1024|g' 	\
	       -e 's|__PAGE_SIZE__|${CONFIG_HAL_PAGE_SIZE}|g' 		\
	       -e 's|__KSTACK_PAGES__|${CONFIG_KSTACK_PAGE_NR}|g'	\
	$< > $@

run: ${QEMU_KERNEL}
	${QEMU} ${QEMU_OPT} -kernel ${QEMU_KERNEL}

run-debug: ${QEMU_KERNEL}
	${QEMU} ${QEMU_OPT} -kernel ${QEMU_KERNEL} ${QEMU_DBG_OPT}

clean:
	${RM} *.o ${lib} asm-offset.s ${top}/include/hal/asm-offset.h

distclean:clean
	${RM} \#* *~
