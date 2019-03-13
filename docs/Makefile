# -*- mode: makefile-gmake; coding:utf-8 -*-
#
#  OS kernel sample
#  Copyright 2018 Takeharu KATO
#
top=..
include ${top}/Makefile.inc
CFLAGS += -I${top}/include
subdirs=taskmonitor-book doxygen

.PHONY: doxygen

all: 
	for dir in ${subdirs} ; do \
	${MAKE} -C $${dir} ;\
	done

doxygen:
	${MAKE} -C doxygen doxygen-doc

clean:
	for dir in ${subdirs} ; do \
	${MAKE} -C $${dir} clean;\
	done
	${RM} *~

distclean:clean
	${RM} \#* *~
