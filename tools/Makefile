top=..
include ${top}/Makefile.inc
CFLAGS += -I${top}/include
subdirs=gdb shell asmoffset templates x64

all: kconfig/mconf
	for dir in ${subdirs-build} ; do 	\
		${MAKE} -C $${dir} all	;	\
	done

kconfig/mconf:
	cd kconfig && $(MAKE) mconf

clean:
	for dir in ${subdirs} ; do 	\
		${MAKE} -C $${dir} distclean;	\
	done
	cd kconfig && $(MAKE) clean

distclean:clean
	for dir in ${subdirs} ; do 	\
		${RM} $${dir}/*~;	\
	done
	cd kconfig && $(MAKE) distclean
	${RM} \#* *~
