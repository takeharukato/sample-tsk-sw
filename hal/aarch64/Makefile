top=../..
include ${top}/Makefile.inc

QEMU ?= qemu-system-aarch64
QEMU_KERNEL ?= ${top}/kernel.elf
QEMU_OPT ?= -M virt -cpu cortex-a57 -m 128 -nographic -serial mon:stdio
QEMU_DBG_OPT := -S -gdb tcp::1234

CFLAGS += -I${top}/include
ASM_OFFSET_CFLAGS = $(shell echo ${CFLAGS}|sed -e 's@-ggdb[0-9]*@@g')
objects = kconsole.o psw.o thr_context.o ctxsw.o
ifeq ($(CONFIG_HAL),y)
objects += boot.o boot64.o data.o exception.o vector.o gic-pl390.o timer.o
endif

lib=libhal.a

all:${top}/include/hal/asm-offset.h kernel.lds ${lib}

${lib}:${objects}
	${AR} ${ARFLAGS} $@ $^
	${RANLIB} $@

${top}/include/hal/asm-offset.h: gen-asm-offset

asm-offset.s: asm-offset.c
	${CC} ${ASM_OFFSET_CFLAGS} -S -o $@ $<

gen-asm-offset: asm-offset.s
	${top}/tools/asmoffset/gen-asm-offset.py \
	-i ${top}/hal/hal/asm-offset.s \
	-o ${top}/include/hal/asm-offset.h

kernel.lds: kernel.lds.in ${top}/.config
	${RM} $@
	${SED} -e 's|__KHEAP_SIZE__|${CONFIG_HEAP_SIZE}*1024*1024|g' 	\
	       -e 's|__PAGE_SIZE__|${CONFIG_HAL_PAGE_SIZE}|g' 		\
	$< > $@

run: ${QEMU_KERNEL}
	${QEMU} ${QEMU_OPT} -kernel ${QEMU_KERNEL}

run-debug: ${QEMU_KERNEL}
	${QEMU} ${QEMU_OPT} -kernel ${QEMU_KERNEL} ${QEMU_DBG_OPT}

clean:
	${RM} *.o ${lib}

distclean:clean
	${RM} *~
