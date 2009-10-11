.SUFFIXES: .asm

LDSCRIPT = krnl1m.ld
NASM     = nasm -f elf
CC       = gcc -Wall -O2 -nostdinc -fno-builtin #-DVERBOSE_MALLOC -DVERBOSE_MEMCPY
LD       = ld -T $(LDSCRIPT) -nostdlib

OBJECT_OBJS =	\
		Objects/frameobject.o \
		Objects/intobject.o \
		Objects/dictobject.o \
		Objects/stringobject.o \
		Objects/typeobject.o \
		Objects/object.o \
		Objects/tupleobject.o \
		Objects/moduleobject.o

PYTHON_OBJS =	\
		Python/pythonrun.o \
		Python/pystate.o \
		Python/import.o \
		Python/marshal.o \
		Python/ceval.o \
		Python/compile.o

MODULE_OBJS =	\
		Modules/main.o \
		Modules/M___main__.o \
		Modules/gcmodule.o

LIB_OBJS =	\
		lib/string.o \
		lib/mem.o

MACH_OBJS =	\
		mach/video.o \
		mach/ports.o \
		mach/malloc.o

OBJS =	\
	$(OBJECT_OBJS) \
	$(PYTHON_OBJS) \
	$(MODULE_OBJS) \
	$(LIB_OBJS) \
	$(MACH_OBJS)

all: c.img

clean:
	-rm -f *.o KERNEL.BIN kernel.lst kernel.map Objects/*.o Python/*.o Modules/*.o lib/*.o mach/*.o

clobber:
	-rm -f *.o KERNEL.BIN kernel.lst kernel.map Objects/*.o Python/*.o Modules/*.o lib/*.o mach/*.o
	-rm -f bochsout.txt c.img grub.img *~ Object/*~ Python/*~ Modules/*~ lib/*~ mach/*~

.asm.o:
	$(NASM) -o $@ $<

.c.o:
	$(CC) -c -I./Include -o $@ $<

c.img: KERNEL.BIN
	MTOOLSRC=mtools.conf mtools -c mcopy -o $< C:

KERNEL.BIN: kstart.o Modules/python.o $(OBJS) $(LDSCRIPT) 
	$(LD) -Map kernel.map -o $@ kstart.o Modules/python.o $(OBJS)
	nm $@ | sort > kernel.lst
	strip $@






