#ifndef Py_STRINGOBJECT_H
#define Py_STRINGOBJECT_H

typedef struct {
  PyObject_VAR_HEAD
  long ob_shash;
  int ob_sstate;
  char ob_sval[1];
} PyStringObject;

#define SSTATE_NOT_INTERNED 0 
#define SSTATE_INTERNED_MORTAL 1
#define SSTATE_INTERNED_IMMORTAL 2

PyAPI_DATA(PyTypeObject) PyBaseString_Type;
PyAPI_DATA(PyTypeObject) PyString_Type;

#define PyString_Check(op) PyObject_TypeCheck(op, &PyString_Type)
#define PyString_CheckExact(op) ((op)->ob_type == &PyString_Type)

PyAPI_FUNC(PyObject *) PyString_FromStringAndSize(const char *, int);
PyAPI_FUNC(PyObject *) PyString_FromString(const char *);

PyAPI_FUNC(char *) PyString_AsString(PyObject *);
PyAPI_FUNC(int) _PyString_Resize(PyObject **, int);

PyAPI_FUNC(int) _PyString_Eq(PyObject *, PyObject *);
PyAPI_FUNC(PyObject *) PyString_Format(PyObject *, PyObject *);

PyAPI_FUNC(void) PyString_InternInPlace(PyObject **);
PyAPI_FUNC(PyObject *) PyString_InternFromString(const char *);

#define PyString_CHECK_INTERNED(op) (((PyStringObject *)(op))->ob_sstate)

#define PyString_AS_STRING(op) (((PyStringObject *)(op))->ob_sval)
#define PyString_GET_SIZE(op)  (((PyStringObject *)(op))->ob_size)

#endif /* !Py_STRINGOBJECT_H */
