#include "Python.h"

PyThreadState *_PyThreadState_Current = NULL;

PyInterpreterState *
PyInterpreterState_New(void)
{
	PyInterpreterState *interp = PyMem_NEW(PyInterpreterState, 1);

	if (interp != NULL) {
		interp->modules = NULL;
		interp->builtins = NULL;
		interp->tstate_head = NULL;
	}

	return interp;
}

void
PyInterpreterState_Clear(PyInterpreterState *interp)
{
	/* TO DO */
}

void
PyInterpreterState_Delete(PyInterpreterState *interp)
{
	/* TO DO */
}

PyThreadState *
PyThreadState_New(PyInterpreterState *interp)
{
	PyThreadState *tstate = PyMem_NEW(PyThreadState, 1);

	if (tstate != NULL) {
		tstate ->interp = interp;

		/* TO DO */
	}

	return tstate;
}

PyThreadState *
PyThreadState_Get(void)
{
	if (_PyThreadState_Current == NULL)
		Py_FatalError("PyThreadState_Get: no current thread");

	return _PyThreadState_Current;
}

PyThreadState *
PyThreadState_Swap(PyThreadState *new)
{
	PyThreadState *old = _PyThreadState_Current;

	_PyThreadState_Current = new;

	return old;
}

PyObject *
PyThreadState_GetDict(void)
{
	if (_PyThreadState_Current == NULL)
		return NULL;

	if (_PyThreadState_Current->dict == NULL) {
		PyObject *d;
		_PyThreadState_Current->dict = d = PyDict_New();
		if (d == NULL)
		  ;/* ERROR */
	}
	return _PyThreadState_Current->dict;
}
