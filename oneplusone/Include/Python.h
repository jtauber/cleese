#ifndef Py_PYTHON_H
#define Py_PYTHON_H

/* from stdio.h */
#define NULL 0
#define EOF (-1)
int printf(const char *, ...);

/* from stddef.h */
typedef long unsigned int size_t;

/* from stdlib.h */
void *malloc(size_t __size);

/* from string.h */
int strcmp(const char *, const char *);
size_t strlen(const char *);
void *memcpy(void *, const void *, size_t);
void *memset(void *, int, size_t);
int *memcmp(const void *, const void *, size_t);

/* from pyport.h */
#define PyAPI_FUNC(RTYPE) RTYPE
#define PyAPI_DATA(RTYPE) extern RTYPE
typedef unsigned int Py_uintptr_t;

/* from pydebug.h */
PyAPI_FUNC(void) Py_FatalError(const char *message);

/* from ? */
#define offsetof(type, member) ( (int) & ((type*)0) -> member )

#define LOG(msg) //print(msg)
#define LOGF(format, arg1) //printf(format, arg1)

#include "pymem.h"

#include "object.h"
#include "objimpl.h"

#include "intobject.h"
#include "stringobject.h"
#include "tupleobject.h"
#include "dictobject.h"
#include "moduleobject.h"

#include "pystate.h"

#include "pythonrun.h"

#include "import.h"

#endif /* !Py_PYTHON_H */
