include ../Makefile.common

INCLUDE_FLAGS = -I./include -I./python/Include

KERNEL_OBJS =	\
		kernel/kstart.o \
		kernel/res.o \
		kernel/main.o \
		kernel/kernel_bytecode.o \
		kernel/isrmodule.o

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

OBJS =	\
		$(KERNEL_OBJS) \
		$(MACH_OBJS) \
		$(LIB_OBJS) \
		$(OBJECT_OBJS) \
		$(PYTHON_OBJS) \
		$(MODULE_OBJS)
