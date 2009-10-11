#ifndef Py_CEVAL_H
#define Py_CEVAL_H

PyAPI_FUNC(PyObject *) PyEval_CallObject(PyObject *, PyObject *);

PyAPI_FUNC(PyObject *) PyEval_GetBuiltins(void);

PyAPI_FUNC(int) Py_AddPendingCall(int (*func)(void *), void *arg);
PyAPI_FUNC(int) Py_MakePendingCalls(void);

PyAPI_DATA(volatile int) _Py_Ticker;
PyAPI_DATA(int) _Py_CheckInterval;

#endif /* !Py_CEVAL_H */
