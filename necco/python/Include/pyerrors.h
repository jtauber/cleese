#ifndef Py_ERRORS_H
#define Py_ERRORS_H


/* Error handling definitions */

PyAPI_FUNC(void) PyErr_SetNone(PyObject *);
PyAPI_FUNC(void) PyErr_SetObject(PyObject *, PyObject *);
PyAPI_FUNC(void) PyErr_SetString(PyObject *, const char *);
PyAPI_FUNC(PyObject *) PyErr_Occurred(void);
PyAPI_FUNC(void) PyErr_Clear(void);
PyAPI_FUNC(void) PyErr_Fetch(PyObject **, PyObject **, PyObject **);
PyAPI_FUNC(void) PyErr_Restore(PyObject *, PyObject *, PyObject *);

/* Error testing and normalization */
PyAPI_FUNC(int) PyErr_GivenExceptionMatches(PyObject *, PyObject *);
PyAPI_FUNC(int) PyErr_ExceptionMatches(PyObject *);
PyAPI_FUNC(void) PyErr_NormalizeException(PyObject**, PyObject**, PyObject**);


/* Predefined exceptions */

PyAPI_DATA(PyObject *) PyExc_Exception;
PyAPI_DATA(PyObject *) PyExc_StopIteration;

PyAPI_DATA(PyObject *) PyExc_AttributeError;

PyAPI_DATA(PyObject *) PyExc_IndexError;
PyAPI_DATA(PyObject *) PyExc_KeyError;

PyAPI_DATA(PyObject *) PyExc_MemoryError;

PyAPI_DATA(PyObject *) PyExc_OverflowError;
PyAPI_DATA(PyObject *) PyExc_RuntimeError;

PyAPI_DATA(PyObject *) PyExc_ReferenceError;
PyAPI_DATA(PyObject *) PyExc_SystemError;

PyAPI_DATA(PyObject *) PyExc_TypeError;

PyAPI_DATA(PyObject *) PyExc_ValueError;
PyAPI_DATA(PyObject *) PyExc_ZeroDivisionError;

PyAPI_DATA(PyObject *) PyExc_MemoryErrorInst;

/* Predefined warning categories */
PyAPI_DATA(PyObject *) PyExc_Warning;
PyAPI_DATA(PyObject *) PyExc_DeprecationWarning;
PyAPI_DATA(PyObject *) PyExc_OverflowWarning;
PyAPI_DATA(PyObject *) PyExc_RuntimeWarning;
PyAPI_DATA(PyObject *) PyExc_FutureWarning;


/* Convenience functions */

PyAPI_FUNC(int) PyErr_BadArgument(void);
PyAPI_FUNC(PyObject *) PyErr_NoMemory(void);

PyAPI_FUNC(PyObject *) PyErr_Format(PyObject *, const char *, ...)
     Py_GCC_ATTRIBUTE((format(printf, 2, 3)));


/* Export the old function so that the existing API remains available: */
PyAPI_FUNC(void) PyErr_BadInternalCall(void);
PyAPI_FUNC(void) _PyErr_BadInternalCall(char *filename, int lineno);
/* Mask the old API with a call to the new API for code compiled under
   Python 2.0: */
#define PyErr_BadInternalCall() _PyErr_BadInternalCall(__FILE__, __LINE__)

/* Function to create a new exception */
PyAPI_FUNC(PyObject *) PyErr_NewException(char *name, PyObject *base,
                                         PyObject *dict);

/* Issue a warning or exception */
PyAPI_FUNC(int) PyErr_Warn(PyObject *, char *);

/* These APIs aren't really part of the error implementation, but
   often needed to format error messages; the native C lib APIs are
   not available on all platforms, which is why we provide emulations
   for those platforms in Python/mysnprintf.c,
   WARNING:  The return value of snprintf varies across platforms; do
   not rely on any particular behavior; eventually the C99 defn may
   be reliable.
*/
PyAPI_FUNC(int) PyOS_snprintf(char *str, size_t size, const char  *format, ...)
			Py_GCC_ATTRIBUTE((format(printf, 3, 4)));
PyAPI_FUNC(int) PyOS_vsnprintf(char *str, size_t size, const char  *format, va_list va)
			Py_GCC_ATTRIBUTE((format(printf, 3, 0)));

#endif /* !Py_ERRORS_H */
