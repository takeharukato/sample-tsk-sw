# -*- mode: makefile-gmake; coding:utf-8 -*-
#
#  Doxygen generator OS kernel sample
#  Copyright 2018 Takeharu KATO
#
top=../..

.PHONY: doxygen

all: doxygen

doxygen-doc: clean
	./scripts/gen-doxygen.sh -s ${top}

clean-gcov:
	${RM} *.gcda *.gcno *.gcov

clean: clean-gcov
	${RM} *-doxygen-doc*.tar.gz *-doxygen-doc-*.log *-doxygen-error.log

distclean: clean
	${RM} *~ GPATH GRTAGS GTAGS


