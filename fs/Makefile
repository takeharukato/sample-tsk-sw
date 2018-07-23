# -*- mode: makefile-gmake; coding:utf-8 -*-
#
#  OS kernel sample
#  Copyright 2018 Takeharu KATO
#
top=..
include ${top}/Makefile.inc
CFLAGS += -I${top}/include
objects=file.o fs.o
lib=libfs.a

all:${lib}

${lib}:${objects}
	${AR} ${ARFLAGS} $@ $^
	${RANLIB} $@

all: ${lib}

clean:
	${RM} ${objects} ${lib}

distclean: clean
	${RM} *~
