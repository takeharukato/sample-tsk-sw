top=..
include ${top}/Makefile.inc
CFLAGS += -I${top}/include -I${top}/user
ifeq ($(CONFIG_LIFEGAME),y)
objects=life.o uprintf.o
else
objects=user.o uprintf.o
endif
lib=libuser.a

all:${lib}

${lib}:${objects}
	${AR} ${ARFLAGS} $@ $^
	${RANLIB} $@
clean:
	${RM} *.o ${lib}

distclean:clean
	${RM} \#* *~
