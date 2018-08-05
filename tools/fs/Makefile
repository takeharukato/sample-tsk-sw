# -*- mode: makefile-gmake; coding:utf-8 -*-
#
#  OS kernel sample
#  Copyright 2018 Takeharu KATO
#
top=../..
include ${top}/Makefile.inc
HOST_CFLAGS += -I${top}/include
fsutils=mkfs
objects=

all: ${fsutils}

mkfs.o: mkfs.c
	${HOST_CC} ${HOST_CFLAGS} -c -o $@ $<

mkfs: mkfs.o ${objects}
	${HOST_CC} ${HOST_CFLAGS} -o $@ mkfs.o ${objects}

clean:
	${RM} *.o ${fsutils} ${objects} *.img

distclean: clean
	${RM} \#* *~
