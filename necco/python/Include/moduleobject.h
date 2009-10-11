#ifndef Py_MODULEOBJECT_H
#define Py_MODULEOBJECT_H

PyAPI_DATA(PyTypeObject) PyModule_Type;

#define PyModule_Check(op) PyObject_TypeCheck(op, &PyModule_Type)

PyAPI_FUNC(PyObject *) PyModule_New(char *);
PyAPI_FUNC(PyObject *) PyModule_GetDict(PyObject *);

#endif /* !Py_MODULEOBJECT_H */
