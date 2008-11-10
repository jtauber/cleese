all: 1p1

PYTHON_OBJS = 	\
				Python/frozenmain.o \
				Python/pystate.o \
				Python/pythonrun.o

OBJS =			\
				$(PYTHON_OBJS)

1p1: Modules/python.o $(OBJS)
	gcc $(FLAGS) -Wall -o $@ Modules/python.o $(OBJS)

.c.o:
	gcc $(FLAGS) -fno-builtin -c -Wall -I./Include -o $@ $<
