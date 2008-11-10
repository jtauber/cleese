all: 1p1

OBJECTS_OBJS =	\
				Objects/dictobject.o \
				Objects/moduleobject.o \
				Objects/object.o \
				Objects/typeobject.o \
				Objects/unicodeobject.o

PYTHON_OBJS = 	\
				Python/frozenmain.o \
				Python/import.o \
				Python/pystate.o \
				Python/pythonrun.o

OBJS =			\
				$(OBJECTS_OBJS) \
				$(PYTHON_OBJS)

1p1: Modules/python.o $(OBJS)
	gcc $(FLAGS) -Wall -o $@ Modules/python.o $(OBJS)

.c.o:
	gcc $(FLAGS) -fno-builtin -c -Wall -I./Include -o $@ $<
