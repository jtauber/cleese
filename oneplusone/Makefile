INCLUDE_FLAGS = -I./Include

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
	kstart.o \
	Modules/python.o \
	$(OBJECT_OBJS) \
	$(PYTHON_OBJS) \
	$(MODULE_OBJS) \
	$(LIB_OBJS) \
	$(MACH_OBJS)


include ../Makefile.common
