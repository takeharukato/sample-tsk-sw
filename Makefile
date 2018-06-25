top=.
include Makefile.inc
targets=kernel

subdirs=common kern hal user
cleandirs=include ${subdirs} tools configs
kernlibs = user/libuser.a kern/libkern.a common/libcommon.a hal/hal/libhal.a
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
	${RM} include/autoconf.h
	${mconf} configs/Config.in || :


include/autoconf.h: .config
	${RM} -f $@
	tools/kconfig/conf-header.sh .config > $@

kernel: kernel.elf kernel.asm
	${CP} $@.elf $@.tmp
	${STRIP} $@.tmp
	${OBJCOPY} -O binary $@.tmp $@
	${RM} $@.tmp

kernel.asm: kernel.elf
	${RM} $@
	${OBJDUMP} -S $< > $@

kernel.elf:include/autoconf.h subsystem
ifeq ($(CONFIG_HAL),y)
	${LD} ${LDFLAGS}  $(shell echo ${CONFIG_HAL_LDFLAGS}) 	\
		-nostdlib -T hal/hal/kernel.lds			\
		-o $@ ${start_obj} 				\
		--start-group ${kernlibs} ${hallibs} --end-group
else
	${CC} ${CFLAGS} -o $@ ${start_obj} ${kernlibs}
endif
hal:
	${MAKE} -C include hal
	${MAKE} -C hal hal
	${MAKE} -C configs hal

subsystem: hal
	for dir in ${subdirs} ; do \
	${MAKE} -C $${dir} ;\
	done

run: hal kernel
	${MAKE} -C hal/hal $@ ;\

run-debug: hal kernel
	${MAKE} -C hal/hal $@ ;\

docs:
	${MAKE} -C docs ;\

clean:
	for dir in ${cleandirs} ; do \
	${MAKE} -C $${dir} clean ;\
	done
	${RM} *.o ${targets} *.tmp *.elf *.asm

distclean:clean
	for dir in ${cleandirs} ; do \
	${MAKE} -C $${dir} distclean ;\
	done
	${RM}  *~ .config* _config GPATH GRTAGS GSYMS GTAGS

gtags:
	${GTAGS} -v

