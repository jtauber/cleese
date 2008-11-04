.SUFFIXES: .asm

LDSCRIPT = krnl1m.ld
NASM     = nasm -f elf
CC       = i386-elf-gcc -Wall -O2 -nostdinc -fno-builtin
LD       = i386-elf-ld -T $(LDSCRIPT) -nostdlib
NM       = i386-elf-nm
STRIP    = i386-elf-strip
OBJS     = kstart.o main.o video.o ports.o

all: KERNEL.BIN

image: cleese.img

clean:
	-rm -f *.o KERNEL.BIN kernel.lst cleese.img

.asm.o:
	$(NASM) -o $@ $<

.c.o:
	$(CC) -c -o $@ $<

KERNEL.BIN: $(OBJS) $(LDSCRIPT)
	$(LD) -o $@ $(OBJS)
	$(NM) $@ | sort > kernel.lst
	$(STRIP) $@

cleese.img: KERNEL.BIN
	hdiutil create -size 5M -fs "MS-DOS" -layout NONE cleese
	mv cleese.dmg cleese.img
	mkdir -p mnt
	mount_msdos -o nosync `hdid -nomount cleese.img` ./mnt
	cp -r boot KERNEL.BIN ./mnt
	umount -f ./mnt
	rm -r ./mnt


