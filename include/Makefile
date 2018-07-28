top=..
include ${top}/Makefile.inc

subdirs=kern hal

all:hal

hal:
	${RM} hal
	${LN} -s hal-${CPU} hal

clean: hal
	for dir in ${subdirs} ; do \
	${MAKE} -C $${dir} clean ;\
	done

distclean:hal clean
	for dir in ${subdirs} ; do \
	${MAKE} -C $${dir} distclean ;\
	done
	${RM} hal
	${RM} \#* *~ autoconf.h
