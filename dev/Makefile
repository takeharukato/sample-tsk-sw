# -*- mode: makefile-gmake; coding:utf-8 -*-
#
#  OS kernel sample
#  Copyright 2014 Takeharu KATO
#
top=..
include ${top}/Makefile.inc
CFLAGS += -I${top}/include
objects=kconsole.o blkio.o
lib=libdev.a

all:${lib}

${lib}:${objects}
	${AR} ${ARFLAGS} $@ $^
	${RANLIB} $@
clean:
	${RM} *.o ${lib}

distclean:clean
	${RM} *~
