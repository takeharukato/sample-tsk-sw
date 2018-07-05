top=.
include Makefile.inc
targets=kernel.elf kernel-dbg.elf kernel.asm kernel.map

subdirs=common kern dev hal user
cleandirs=include ${subdirs} tools configs
kernlibs = user/libuser.a kern/libkern.a common/libcommon.a dev/libdev.a hal/hal/libhal.a
mconf=tools/kconfig/mconf

all:${targets}

start_obj =
ifeq ($(CONFIG_HAL),y)
start_obj += hal/hal/boot.o
endif
start_obj += kern/main.o

${mconf}:
	${MAKE} -C tools

menuconfig:hal configs/Config.in ${mconf}
	${RM} include/kern/autoconf.h
	${mconf} configs/Config.in || :


include/kern/autoconf.h: .config
	${RM} -f $@
	tools/kconfig/conf-header.sh .config > $@

kernel.asm: kernel.elf
	${RM} $@
	${OBJDUMP} -S $< > $@

kernel.map: kernel.elf
	${RM} $@
	${NM} $< |${SORT} > $@

kernel.elf: kernel-dbg.elf
	${CP}	$< $@
	${STRIP} -g $@

kernel-dbg.elf: include/kern/autoconf.h subsystem
ifeq ($(CONFIG_HAL),y)
	${CC} ${LDFLAGS}  $(shell echo ${CONFIG_HAL_LDFLAGS}) 	\
		-nostdlib -T hal/hal/kernel.lds			\
		-o $@ ${start_obj} 				\
		-Wl,--start-group ${kernlibs} ${hallibs} -Wl,--end-group
else
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${start_obj} -Wl,--start-group ${kernlibs} -Wl,--end-group
endif
hal:
	${MAKE} -C include hal
	${MAKE} -C hal hal
	${MAKE} -C configs hal

subsystem: hal
	for dir in ${subdirs} ; do \
	${MAKE} -C $${dir} ;\
	done

run: hal kernel.elf
	${MAKE} -C hal/hal $@ ;\

run-debug: hal kernel.elf
	${MAKE} -C hal/hal $@ ;\

docs:
	${MAKE} -C docs ;\

clean:
	for dir in ${cleandirs} ; do \
	${MAKE} -C $${dir} clean ;\
	done
	${RM} *.o ${targets} *.tmp *.elf *.asm *.map *.iso

distclean:clean
	for dir in ${cleandirs} ; do \
	${MAKE} -C $${dir} distclean ;\
	done
	${RM}  \#* *~ .config* _config GPATH GRTAGS GSYMS GTAGS *.log

gtags:
	${GTAGS} -v

