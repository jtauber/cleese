#ifndef Py_OPCODE_H
#define Py_OPCODE_H

#define POP_TOP		1
#define ROT_TWO		2
#define ROT_THREE	3
#define DUP_TOP		4
#define ROT_FOUR	5

#define UNARY_NEGATIVE	11
#define UNARY_NOT	12

#define BINARY_MULTIPLY	20
#define BINARY_DIVIDE	21
#define BINARY_MODULO	22
#define BINARY_ADD      23
#define BINARY_SUBTRACT 24
#define BINARY_SUBSCR   25

#define SLICE           30
/* Also uses 31-33 */

#define STORE_SLICE	40

#define STORE_SUBSCR	60

#define BINARY_LSHIFT	62
#define BINARY_RSHIFT	63
#define BINARY_AND	64

#define BINARY_OR       66
#define GET_ITER	68

#define PRINT_ITEM      71
#define PRINT_NEWLINE   72

#define BREAK_LOOP      80

#define RETURN_VALUE    83

#define POP_BLOCK	87

#define HAVE_ARGUMENT   90

#define STORE_NAME      90

#define UNPACK_SEQUENCE	92	/* Number of sequence items */
#define FOR_ITER	93

#define STORE_GLOBAL	97	/* "" */

#define LOAD_CONST	100	/* Index in const list */
#define LOAD_NAME	101	/* Index in name list */

#define BUILD_TUPLE	102	/* Number of tuple items */
#define BUILD_LIST	103	/* Number of list items */
#define BUILD_MAP	104	/* Always zero for now */
#define LOAD_ATTR	105	/* Index in name list */
#define COMPARE_OP	106	/* Comparison operator */

#define IMPORT_NAME	107	/* Index in name list */

#define JUMP_FORWARD	110	/* Number of bytes to skip */
#define JUMP_IF_FALSE	111	/* "" */
#define JUMP_IF_TRUE	112	/* "" */

#define JUMP_ABSOLUTE	113	/* Target byte offset from beginning of code */

#define LOAD_GLOBAL	116	/* Index in name list */

#define CONTINUE_LOOP	119	/* Start of loop (absolute) */
#define SETUP_LOOP	120	/* Target address (absolute) */
#define SETUP_EXCEPT	121	/* "" */
#define SETUP_FINALLY	122	/* "" */

#define LOAD_FAST	124	/* Local variable number */
#define STORE_FAST	125	/* Local variable number */

#define SET_LINENO	127

/* CALL_FUNCTION_XXX opcodes defined below depend on this definition */
#define CALL_FUNCTION	131	/* #args + (#kwargs<<8) */
#define MAKE_FUNCTION	132	/* #defaults */

#define HAS_ARG(op) ((op) >= HAVE_ARGUMENT)

/* Comparison operator codes (argument to COMPARE_OP) */
enum cmp_op {PyCmp_LT=Py_LT, PyCmp_LE=Py_LE, PyCmp_EQ=Py_EQ, PyCmp_NE=Py_NE, PyCmp_GT=Py_GT, PyCmp_GE=Py_GE,
	     PyCmp_IN, PyCmp_NOT_IN, PyCmp_IS, PyCmp_IS_NOT, PyCmp_EXC_MATCH, PyCmp_BAD};

#endif /* !Py_OPCODE_H */
