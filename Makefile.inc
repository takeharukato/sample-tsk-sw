CPU ?= `uname -m|sed -e "s/amd64/x86-64/" -e "s/x86_64/x86-64/" -e "s/i?86/ia32/"`

CC=${CROSS_COMPILE}gcc
AR=${CROSS_COMPILE}ar
LD=${CROSS_COMPILE}ld
RANLIB=${CROSS_COMPILE}ranlib
STRIP=${CROSS_COMPILE}strip
OBJCOPY=${CROSS_COMPILE}objcopy
OBJDUMP=${CROSS_COMPILE}objdump
NM=${CROSS_COMPILE}nm
SED=sed
SORT=sort
GTAGS=gtags
CP=cp
RM=rm -f
LN=ln
ARFLAGS=rvc
OPT_FLAGS=-O0 -g
CFLAGS=${OPT_FLAGS} $(shell echo ${CONFIG_HAL_CFLAGS}) $(shell echo ${CONFIG_HAL_OPT_FLAGS})
LDFLAGS=

.PHONY: clean distclean hal tools menuconfig gtags

.SUFFIXES: .o .c .S

.c.o:
	${CC} ${CFLAGS} -c -o $@ $<

.S.o:
	${CC} ${CFLAGS} -c -o $@ $<

-include ${top}/.config

