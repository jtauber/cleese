.SUFFIXES:	.boa

CC       = gcc -Wall -O2 -nostdinc -fno-builtin
CC	 += -fno-strict-aliasing
#CC      += -fno-leading-underscore
#CC	 += -DVERBOSE_MALLOC -DVERBOSE_MEMCPY -DVERBOSE_MARSHAL
LD       = ld -nostdlib
SYMTAB	 = -defsym symtab=defsyms
PYTHON	 = python

# gcc on cygwin doesn't seem to obey '-fno-leading-underscore'
#-------------------------------------------------------------
CYGWIN_KLUDGE = objcopy --remove-leading-char $@ tmp.o; mv tmp.o $@

LED_PY = \
		kernel/led.py \
		kernel/rtc.py \
		kernel/bochs/pyvga.py

ECHOISR_PY = \
		kernel/echoisr.py \
		kernel/keyb.py \
		kernel/rtc.py \
		kernel/bochs/pyvga.py \
		kernel/bochs/comport.py \
		kernel/bochs/pyfont.py \
		kernel/bochs/py8042.py

SOKO_PY	= \
		kernel/soko.py \
		kernel/keyb.py \
		kernel/bochs/pyvga.py \
		kernel/bochs/py8042.py

MONKEY_PY = \
		kernel/monkey.py \
		kernel/keyb.py \
		kernel/bochs/pyvga.py \
		kernel/bochs/py8042.py \
		kernel/bochs/pybeep.py

KERNEL_PY = $(ECHOISR_PY)
#KERNEL_PY = $(LED_PY)
#KERNEL_PY = kernel/listtest.py
#KERNEL_PY = kernel/stacktest.py

#KERNEL_PY = $(SOKO_PY)
#SYMTAB    = -defsym symtab=sokosyms
#RES_OBJS  = kernel/sokores.o
#kernel/sokores.o: kernel/sokobg.dat

#KERNEL_PY = $(MONKEY_PY)
#SYMTAB    = -defsym symtab=sokosyms
#RES_OBJS  = kernel/monkres.o
#kernel/monkres.o: kernel/sokobg.dat

KERNEL_OBJS =	\
		kernel/kernel_bytecode.o \
		kernel/blitmodule.o \
		kernel/bufmodule.o \
		kernel/stackmodule.o 

KERNEL_OBJS +=	\
		kernel/bochs/init.o \
		kernel/bochs/israbc.o \
		kernel/bochs/isrmodule.o \
		kernel/bochs/portsmodule.o \
		kernel/bochs/stackabc.o \
		kernel/bochs/vgadata.o

PYTHON_OBJS =	\
		python/Python/pythonrun.o \
		python/Python/pystate.o \
		python/Python/import.o \
		python/Python/marshal.o \
		python/Python/ceval.o \
		python/Python/compile.o \
		python/Python/bltinmodule.o \
		python/Python/getargs.o \
		python/Python/modsupport.o \
		python/Python/errors.o \
		python/Python/exceptions.o \
		python/Python/mysnprintf.o \
		python/Python/mystrtoul.o \
		python/Python/structmember.o

PYTHON_OBJS +=	\
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
		python/Objects/bufferobject.o \
		python/Objects/longobject.o \
		python/Objects/floatobject.o \
		python/Objects/iterobject.o \
		python/Objects/sliceobject.o \
		python/Objects/cellobject.o \
		python/Objects/descrobject.o \
		python/Objects/weakrefobject.o \
		python/Objects/enumobject.o

PYTHON_OBJS +=	\
		python/Modules/gcmodule.o \
		python/Modules/main.o

LDSCRIPT =	lib/bochs/cleese.ld
CRT0	 =	lib/bochs/crt0.o

LIB_OBJS =	\
		lib/string.o \
		lib/malloc.o \
		lib/ctype.o \
		lib/snprintf.o \
		lib/qsort.o

LIB_OBJS +=	\
		lib/bochs/video.o \
		lib/bochs/ports.o \
		lib/bochs/mem.o

OBJS =		\
		$(KERNEL_OBJS) \
		$(PYTHON_OBJS) \
		$(LIB_OBJS) \
		$(RES_OBJS)

all: grub

clean:
	-rm -f KERNEL.BIN cleese cleese.map kernel_bytecode.c
	-rm -f kernel/kernel_bytecode.c
	find . -name '*.o' -exec rm -f {} ';'
	find . -name '*.s' -exec rm -f {} ';'

clobber: clean
	-rm -f bochsout.txt
	-rm -f disk/*.img
	find . -name '*~' -exec rm -f {} ';'

.c.o:
	$(CC) -c -I./include -I./python/Include -o $@ $<
	$(CYGWIN_KLUDGE)

.boa.o:
	$(PYTHON) util/boa $< | as -o $@

kernel/kernel_bytecode.c: $(KERNEL_PY)
	$(PYTHON) util/freeze.py $^
	mv kernel_bytecode.c $@

cleese: $(LDSCRIPT) $(CRT0) $(OBJS) # order is important!
	$(LD) -Map cleese.map $(SYMTAB) -o $@ -T $^

grub: cleese FORCE
	objcopy -R .note -R .comment -S -O elf32-i386 $< KERNEL.BIN
	util/push KERNEL.BIN $@

bootp: cleese FORCE
	objcopy -R .note -R .comment -S -O binary $< KERNEL.BIN
	util/push KERNEL.BIN $@

FORCE:

p:	$(PYTHON_OBJS)
k:	$(KERNEL_OBJS)
l:	$(LIB_OBJS)
