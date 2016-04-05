top=..
include ${top}/Makefile.inc
CFLAGS += -I${top}/include
objects=bitops.o id_bitmap.o string.o malloc.o chunk.o heap.o list.o printf.o
lib=libcommon.a

all:${lib}

${lib}:${objects}
	${AR} ${ARFLAGS} $@ $^
	${RANLIB} $@
clean:
	${RM} *.o ${lib}

distclean:clean
	${RM} *~
