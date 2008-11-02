.SUFFIXES: .asm

LDSCRIPT = krnl1m.ld
NASM     = nasm -f elf
CC       = toolchain/bin/i386-elf-gcc -Wall -O2 -nostdinc -fno-builtin
LD       = toolchain/bin/i386-elf-ld -T $(LDSCRIPT) -nostdlib
NM       = toolchain/bin/i386-elf-nm
STRIP    = toolchain/bin/i386-elf-strip
OBJS     = kstart.o main.o video.o ports.o

all: KERNEL.BIN

clean:
	-rm -f *.o KERNEL.BIN kernel.lst

.asm.o:
	$(NASM) -o $@ $<

.c.o:
	$(CC) -c -o $@ $<

KERNEL.BIN: $(OBJS) $(LDSCRIPT)
	$(LD) -o $@ $(OBJS)
	$(NM) $@ | sort > kernel.lst
	$(STRIP) $@





