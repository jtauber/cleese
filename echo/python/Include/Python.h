#ifndef Py_PYTHON_H
#define Py_PYTHON_H

#include "stdarg.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/* from pyport.h */
#define PyAPI_FUNC(RTYPE) RTYPE
#define PyAPI_DATA(RTYPE) extern RTYPE
typedef unsigned int Py_uintptr_t;
#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

/* from pydebug.h */
PyAPI_FUNC(void) Py_FatalError(const char *message);

/* from ? */
#define offsetof(type, member) ( (int) & ((type*)0) -> member )

#define LOG(msg) //printf(msg)
#define LOGF(format, arg1) //printf(format, arg1)

#include "pymem.h"

#include "object.h"
#include "objimpl.h"

#include "intobject.h"
#include "boolobject.h"
#include "bufferobject.h"
#include "stringobject.h"
#include "tupleobject.h"
#include "listobject.h"
#include "dictobject.h"
#include "methodobject.h"
#include "moduleobject.h"
#include "funcobject.h"
#include "classobject.h"
#include "descrobject.h"

#include "pystate.h"

#include "modsupport.h"
#include "pythonrun.h"
#include "ceval.h"

#include "import.h"

#include "abstract.h"

#endif /* !Py_PYTHON_H */
