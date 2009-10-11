#ifndef Py_OPCODE_H
#define Py_OPCODE_H

#define BINARY_ADD 23

#define PRINT_ITEM 71
#define PRINT_NEWLINE 72

#define RETURN_VALUE 83

#define HAVE_ARGUMENT 90

#define LOAD_CONST 100

#define HAS_ARG(op) ((op) >= HAVE_ARGUMENT)

#endif /* !Py_OPCODE_H */
