/* Boolean object interface */

#ifndef Py_BOOLOBJECT_H
#define Py_BOOLOBJECT_H

/* Py_False and Py_True are the only two bools in existence.
Don't forget to apply Py_INCREF() when returning either!!! */

/* Don't use these directly */
PyAPI_DATA(PyIntObject) _Py_ZeroStruct, _Py_TrueStruct;

/* Use these macros */
#define Py_False ((PyObject *) &_Py_ZeroStruct)
#define Py_True ((PyObject *) &_Py_TrueStruct)


#endif /* !Py_BOOLOBJECT_H */
