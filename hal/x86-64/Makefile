top=../..
include ${top}/Makefile.inc
CFLAGS += -I${top}/include
objects = thr_context.o ctxsw_glue.o kconsole.o
ifeq ($(CONFIG_HAL),y)
objects += boot.o boot64.o raw_console.o machdep.o segment.o
endif

lib=libhal.a

all: kernel.lds ${lib}

${lib}:${objects}
	${AR} ${ARFLAGS} $@ $^
	${RANLIB} $@

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

clean:
	${RM} *.o ${lib}

distclean:clean
	${RM} *~
