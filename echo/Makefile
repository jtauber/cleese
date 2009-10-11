.SUFFIXES:	.boa

LDSCRIPT = cleese.ld
NASM     = nasm -f elf
CC       = i386-elf-gcc -Wall -O2 -nostdinc -fno-builtin
LD       = i386-elf-ld -T $(LDSCRIPT) -nostdlib
NM       = i386-elf-nm
STRIP    = i386-elf-strip
AS       = i386-elf-as

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

all: KERNEL.BIN

clean:
	-rm -f KERNEL.BIN
	-rm -f kernel.lst kernel.map
	find . -name '*.o' -exec rm -f {} ';'
	find . -name '*.s' -exec rm -f {} ';'

.c.o:
	$(CC) -c -I./include -I./python/Include -o $@ $<

.boa.o:
	python util/boa < $< | $(AS) -o $@


KERNEL.BIN: kernel/kstart.o $(OBJS) $(LDSCRIPT) 
	$(LD) -Map kernel.map -o $@ kernel/kstart.o $(OBJS)
	$(NM) $@ | sort > kernel.lst
	$(STRIP) $@




images: grub.img cleese.img

.asm.o:
	$(NASM) -o $@ $<

grub.img: boot/grub/stage1 boot/grub/stage2
	cat boot/grub/stage1 boot/grub/stage2 > grub.img

cleese.img: KERNEL.BIN
	hdiutil create -size 5M -fs "MS-DOS" -layout NONE cleese
	mv cleese.dmg cleese.img
	mkdir -p mnt
	mount_msdos -o nosync `hdid -nomount cleese.img` ./mnt
	cp -r boot KERNEL.BIN ./mnt
	umount -f ./mnt
	rm -r ./mnt

update-image:
	mkdir -p mnt
	mount_msdos -o nosync `hdid -nomount cleese.img` ./mnt
	cp KERNEL.BIN ./mnt
	umount -f ./mnt
	rm -r ./mnt



