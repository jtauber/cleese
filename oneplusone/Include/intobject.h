#ifndef Py_INTOBJECT_H
#define Py_INTOBJECT_H

typedef struct {
	PyObject_HEAD
	long ob_ival;
} PyIntObject;

PyAPI_DATA(PyTypeObject) PyInt_Type;

#define PyInt_Check(op) PyObject_TypeCheck(op, &PyInt_Type)
#define PyInt_CheckExact(op) ((op)->ob_type == &PyInt_Type)

#define PyInt_AS_LONG(op) (((PyIntObject *)(op))->ob_ival)

PyAPI_FUNC(PyObject *) PyInt_FromLong(long);

#endif /* !Py_INTOBJECT_H */
