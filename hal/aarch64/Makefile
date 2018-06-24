top=../..
include ${top}/Makefile.inc

QEMU ?= qemu-system-aarch64
QEMU_KERNEL ?= ${top}/kernel.elf
QEMU_OPT ?= -M virt -cpu cortex-a57 -m 128 -nographic -serial mon:stdio
QEMU_DBG_OPT := -S -gdb tcp::1234

CFLAGS += -I${top}/include
objects = kconsole.o psw.o thr_context.o ctxsw.o
ifeq ($(CONFIG_HAL),y)
objects += boot.o boot64.o #machdep.o segment.o
endif

lib=libhal.a

all:${lib}

${lib}:${objects}
	${AR} ${ARFLAGS} $@ $^
	${RANLIB} $@

run: ${QEMU_KERNEL}
	${QEMU} ${QEMU_OPT} -kernel ${QEMU_KERNEL}

run-debug: ${QEMU_KERNEL}
	${QEMU} ${QEMU_OPT} -kernel ${QEMU_KERNEL} ${QEMU_DBG_OPT}

clean:
	${RM} *.o ${lib}

distclean:clean
	${RM} *~
