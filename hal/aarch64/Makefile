top=../..
include ${top}/Makefile.inc
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
clean:
	${RM} *.o ${lib}

distclean:clean
	${RM} *~
