#ifndef Py_PYTHON_H
#define Py_PYTHON_H

#include "stdarg.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/* from pyport.h */
#ifdef SIGNED_RIGHT_SHIFT_ZERO_FILLS
#define Py_ARITHMETIC_RIGHT_SHIFT(TYPE, I, J) \
	((I) < 0 ? ~((~(unsigned TYPE)(I)) >> (J)) : (I) >> (J))
#else
#define Py_ARITHMETIC_RIGHT_SHIFT(TYPE, I, J) ((I) >> (J))
#endif

#define PyAPI_FUNC(RTYPE) RTYPE
#define PyAPI_DATA(RTYPE) extern RTYPE
typedef unsigned int Py_uintptr_t;
#ifndef INT_MAX
#define INT_MAX 2147483647
#endif
#ifndef UCHAR_MAX
#define UCHAR_MAX 255
#endif
#ifndef SHRT_MIN
#define SHRT_MIN (-32768)
#endif
#ifndef SHRT_MAX
#define SHRT_MAX 32767
#endif
#ifndef INT_MIN
#define INT_MIN (-INT_MAX-1)
#endif

#define SIZEOF_LONG 4
#define LONG_MAX 0X7FFFFFFFL

#ifndef LONG_MIN
#define LONG_MIN (-LONG_MAX-1)
#endif

#ifndef LONG_BIT
#define LONG_BIT (8 * SIZEOF_LONG)
#endif

#define SIZEOF_VOID_P 4
#define SIZEOF_INT 4

/*
 * Hide GCC attributes from compilers that don't support them.
 */
#if (!defined(__GNUC__) || __GNUC__ < 2 || \
     (__GNUC__ == 2 && __GNUC_MINOR__ < 7) ) && \
    !defined(RISCOS)
#define Py_GCC_ATTRIBUTE(x)
#else
#define Py_GCC_ATTRIBUTE(x) __attribute__(x)
#endif

/* end from pyport.h */


#define assert(test) if(!(test)) printf("ASSERT FAILED %s %d\n", __FILE__, __LINE__);

/* from pydebug.h */
PyAPI_FUNC(void) Py_FatalError(const char *message);

/* from ? */
#define offsetof(type, member) ( (int) & ((type*)0) -> member )

int errno;

/* end of from ? */

#define LOG(msg) //print(msg)
#define LOGF(format, arg1) //printf(format, arg1)

#include "pymem.h"

#include "object.h"
#include "objimpl.h"

#include "intobject.h"
#include "boolobject.h"
#include "longobject.h"
#include "floatobject.h"
#include "bufferobject.h"
#include "stringobject.h"
#include "tupleobject.h"
#include "listobject.h"
#include "dictobject.h"
#include "enumobject.h"
#include "methodobject.h"
#include "moduleobject.h"
#include "funcobject.h"
#include "classobject.h"
#include "sliceobject.h"
#include "cellobject.h"
#include "iterobject.h"
#include "descrobject.h"
#include "weakrefobject.h"

#include "pyerrors.h"

#include "pystate.h"

#include "modsupport.h"
#include "pythonrun.h"
#include "ceval.h"

#include "import.h"

#include "abstract.h"

/* Convert a possibly signed character to a nonnegative int */
/* XXX This assumes characters are 8 bits wide */
#ifdef __CHAR_UNSIGNED__
#define Py_CHARMASK(c)		(c)
#else
#define Py_CHARMASK(c)		((c) & 0xff)
#endif

#include "pyfpe.h"
#define ERANGE 34

/* Define macros for inline documentation. */
#define PyDoc_VAR(name) static char name[]
#define PyDoc_STRVAR(name,str) PyDoc_VAR(name) = PyDoc_STR(str)
#ifdef WITH_DOC_STRINGS
#define PyDoc_STR(str) str
#else
#define PyDoc_STR(str) ""
#endif



#endif /* !Py_PYTHON_H */
