.SUFFIXES: .asm

LDSCRIPT = krnl1m.ld
NASM     = nasm -f elf
CC       = gcc -Wall -O2 -nostdinc -fno-builtin
LD       = ld -T $(LDSCRIPT) -nostdlib
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
	nm $@ | sort > kernel.lst
	strip $@





