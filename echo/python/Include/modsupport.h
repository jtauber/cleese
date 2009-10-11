
#ifndef Py_MODSUPPORT_H
#define Py_MODSUPPORT_H

/* Module support interface */

PyAPI_FUNC(int) PyArg_UnpackTuple(PyObject *, char *, int, int, ...);
PyAPI_FUNC(PyObject *) Py_BuildValue(char *, ...);

#define PYTHON_API_VERSION 1012

PyAPI_FUNC(PyObject *) Py_InitModule4(char *name, PyMethodDef *methods,
                                            char *doc, PyObject *self,
                                            int apiver);

#endif /* !Py_MODSUPPORT_H */
