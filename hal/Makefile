top=..
include ${top}/Makefile.inc

all:hal
	${MAKE} -C hal

hal:
	${RM} hal
	${LN} -s ${CPU} hal

clean:hal
	${MAKE} -C hal clean

distclean:hal clean
	${MAKE} -C hal distclean
	${RM} \#* *~
	${RM} hal
