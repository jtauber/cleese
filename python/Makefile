all: 1p1

MODULES_OBJS =	\
				Modules/gcmodule.o

OBJECTS_OBJS =	\
				Objects/abstract.o \
				Objects/boolobject.o \
				Objects/bytesobject.o \
				Objects/codeobject.o \
				Objects/dictobject.o \
				Objects/frameobject.o \
				Objects/listobject.o \
				Objects/longobject.o \
				Objects/methodobject.o \
				Objects/moduleobject.o \
				Objects/object.o \
				Objects/tupleobject.o \
				Objects/typeobject.o \
				Objects/unicodeobject.o

PYTHON_OBJS = 	\
				Python/bltinmodule.o \
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
# -fprofile-arcs -ftest-coverage
#
# then after compiling and executing, can do things like:
# $ gcov -p Python/marshal.c -o Python/marshal

1p1: oneplusone.o $(OBJS)
	gcc $(FLAGS) -o $@ oneplusone.o $(OBJS)

.c.o:
	gcc $(FLAGS) -fno-builtin -c -I./Include -o $@ $<

clean:
	find . -name "*.o" -exec rm -f {} \;
	find . -name "*.gcov" -exec rm -f {} \;
	find . -name "*.gcda" -exec rm -f {} \;
	find . -name "*.gcno" -exec rm -f {} \;
	rm -f 1p1
