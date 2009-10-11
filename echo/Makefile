.SUFFIXES:	.boa

LDSCRIPT = cleese.ld
CC       = gcc -Wall -O2 -nostdinc -fno-builtin #-DVERBOSE_MALLOC -DVERBOSE_MEMCPY -DVERBOSE_MARSHAL
LD       = ld -T $(LDSCRIPT) -nostdlib

KERNEL_OBJS =	\
		kernel/res.o \
		kernel/main.o \
		kernel/kernel_bytecode.o

MACH_OBJS =	\
		mach/video.o \
		mach/ports.o \
		mach/malloc.o

LIB_OBJS =	\
		lib/string.o \
		lib/ctype.o \
		lib/mem.o

OBJECT_OBJS =	\
		python/Objects/frameobject.o \
		python/Objects/intobject.o \
		python/Objects/dictobject.o \
		python/Objects/stringobject.o \
		python/Objects/typeobject.o \
		python/Objects/object.o \
		python/Objects/tupleobject.o \
		python/Objects/moduleobject.o \
		python/Objects/methodobject.o \
		python/Objects/abstract.o \
		python/Objects/classobject.o \
		python/Objects/funcobject.o \
		python/Objects/boolobject.o \
		python/Objects/listobject.o \
		python/Objects/bufferobject.o

PYTHON_OBJS =	\
		python/Python/pythonrun.o \
		python/Python/pystate.o \
		python/Python/import.o \
		python/Python/marshal.o \
		python/Python/ceval.o \
		python/Python/compile.o \
		python/Python/bltinmodule.o \
		python/Python/getargs.o \
		python/Python/modsupport.o

MODULE_OBJS =	\
		python/Modules/gcmodule.o

OBJS =		\
		$(KERNEL_OBJS) \
		$(MACH_OBJS) \
		$(LIB_OBJS) \
		$(OBJECT_OBJS) \
		$(PYTHON_OBJS) \
		$(MODULE_OBJS)

all: c.img

clean:
	-rm -f KERNEL.BIN
	-rm -f kernel.lst kernel.map
	find . -name '*.o' -exec rm -f {} ';'
	find . -name '*.s' -exec rm -f {} ';'

clobber: clean
	-rm -f bochsout.txt
	-rm -f disk/*.img
	find . -name '*~' -exec rm -f {} ';'

.c.o:
	$(CC) -c -I./include -I./python/Include -o $@ $<

.boa.o:
	python util/boa < $< | as -o $@

c.img: KERNEL.BIN
	MTOOLSRC=disk/mtools.conf mtools -c mcopy -o $< C:

KERNEL.BIN: kernel/kstart.o $(OBJS) $(LDSCRIPT) 
	$(LD) -Map kernel.map -o $@ kernel/kstart.o $(OBJS)
	nm $@ | sort > kernel.lst
	strip $@
