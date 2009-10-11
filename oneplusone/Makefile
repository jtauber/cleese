INCLUDE_FLAGS = -I./python/Include

OBJECT_OBJS =	\
		python/Objects/frameobject.o \
		python/Objects/intobject.o \
		python/Objects/dictobject.o \
		python/Objects/stringobject.o \
		python/Objects/typeobject.o \
		python/Objects/object.o \
		python/Objects/tupleobject.o \
		python/Objects/moduleobject.o

PYTHON_OBJS =	\
		python/Python/pythonrun.o \
		python/Python/pystate.o \
		python/Python/import.o \
		python/Python/marshal.o \
		python/Python/ceval.o \
		python/Python/compile.o

MODULE_OBJS =	\
		python/Modules/main.o \
		python/Modules/M___main__.o \
		python/Modules/gcmodule.o

LIB_OBJS =	\
		lib/string.o \
		lib/mem.o

MACH_OBJS =	\
		mach/video.o \
		mach/ports.o \
		mach/malloc.o

OBJS =	\
	kstart.o \
	python/Modules/python.o \
	$(OBJECT_OBJS) \
	$(PYTHON_OBJS) \
	$(MODULE_OBJS) \
	$(LIB_OBJS) \
	$(MACH_OBJS)


include ../Makefile.common
