#ifndef Py_OPCODE_H
#define Py_OPCODE_H

#define POP_TOP		1

#define UNARY_NOT	12

#define BINARY_MODULO	22
#define BINARY_ADD 23
#define BINARY_SUBSCR   25

#define STORE_SLICE	40

#define STORE_SUBSCR	60

#define BINARY_AND	64

#define PRINT_ITEM 71
#define PRINT_NEWLINE 72

#define RETURN_VALUE 83

#define POP_BLOCK	87

#define HAVE_ARGUMENT 90

#define STORE_NAME     90

#define LOAD_CONST	100	/* Index in const list */
#define LOAD_NAME	101	/* Index in name list */

#define LOAD_ATTR	105	/* Index in name list */

#define IMPORT_NAME	107	/* Index in name list */

#define JUMP_FORWARD	110	/* Number of bytes to skip */
#define JUMP_IF_FALSE	111	/* "" */

#define JUMP_ABSOLUTE	113	/* Target byte offset from beginning of code */

#define LOAD_GLOBAL	116	/* Index in name list */

#define SETUP_LOOP	120	/* Target address (absolute) */

#define LOAD_FAST	124	/* Local variable number */
#define STORE_FAST	125	/* Local variable number */

#define SET_LINENO	127

/* CALL_FUNCTION_XXX opcodes defined below depend on this definition */
#define CALL_FUNCTION	131	/* #args + (#kwargs<<8) */
#define MAKE_FUNCTION	132	/* #defaults */

#define HAS_ARG(op) ((op) >= HAVE_ARGUMENT)

#endif /* !Py_OPCODE_H */
