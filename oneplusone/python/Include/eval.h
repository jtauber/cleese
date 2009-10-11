#ifndef Py_EVAL_H
#define Py_EVAL_H

PyAPI_FUNC(PyObject *) PyEval_EvalCode(PyCodeObject *, PyObject *, PyObject *);

PyAPI_FUNC(PyObject *) PyEval_EvalCodeEx(PyCodeObject *co,
					 PyObject *globals,
					 PyObject *locals,
					 PyObject **args, int argc,
					 PyObject **kwds, int kwdc,
					 PyObject **defs, int defc,
					 PyObject *closure);

#endif /* !Py_EVAL_H */
