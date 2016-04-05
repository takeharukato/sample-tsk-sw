top=../..
include ${top}/Makefile.inc
CFLAGS += -I${top}/include
objects = psw.o thr_context.o ctxsw_glue.o kconsole.o
ifeq ($(CONFIG_HAL),y)
objects += boot.o boot64.o raw_console.o machdep.o segment.o
endif

lib=libhal.a

all:${lib}

${lib}:${objects}
	${AR} ${ARFLAGS} $@ $^
	${RANLIB} $@
clean:
	${RM} *.o ${lib}

distclean:clean
	${RM} *~
