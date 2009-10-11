#ifndef Py_CEVAL_H
#define Py_CEVAL_H

PyAPI_FUNC(PyObject *) PyEval_CallObject(PyObject *, PyObject *);

PyAPI_FUNC(PyObject *) PyEval_GetBuiltins(void);
PyAPI_FUNC(PyObject *) PyEval_GetGlobals(void);
PyAPI_FUNC(PyObject *) PyEval_GetLocals(void);
PyAPI_FUNC(struct _frame *) PyEval_GetFrame(void);

PyAPI_FUNC(int) Py_AddPendingCall(int (*func)(void *), void *arg);
PyAPI_FUNC(int) Py_MakePendingCalls(void);

PyAPI_DATA(volatile int) _Py_Ticker;
PyAPI_DATA(int) _Py_CheckInterval;

PyAPI_FUNC(int) _PyEval_SliceIndex(PyObject *, int *);

#endif /* !Py_CEVAL_H */
