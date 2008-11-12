all: 1p1

MODULES_OBJS =	\
				Modules/gcmodule.o

OBJECTS_OBJS =	\
				Objects/boolobject.o \
				Objects/dictobject.o \
				Objects/listobject.o \
				Objects/moduleobject.o \
				Objects/object.o \
				Objects/typeobject.o \
				Objects/unicodeobject.o

PYTHON_OBJS = 	\
				Python/ceval.o \
				Python/errors.o \
				Python/frozenmain.o \
				Python/import.o \
				Python/marshal.o \
				Python/pystate.o \
				Python/pythonrun.o

OBJS =			\
				$(MODULES_OBJS) \
				$(OBJECTS_OBJS) \
				$(PYTHON_OBJS)

FLAGS = -g -fnested-functions -Wreturn-type -Wimplicit-function-declaration

1p1: oneplusone.o $(OBJS)
	gcc $(FLAGS) -o $@ oneplusone.o $(OBJS)

.c.o:
	gcc $(FLAGS) -fno-builtin -c -I./Include -o $@ $<

clean:
	find . -name "*.o" -exec rm -f {} \;
	rm -f 1p1
