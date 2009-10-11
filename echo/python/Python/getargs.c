
/* New getargs implementation */

#include "Python.h"

int
PyArg_UnpackTuple(PyObject *args, char *name, int min, int max, ...)
{
	int i, l;
	PyObject **o;
	va_list vargs;

	va_start(vargs, max);

	if (!PyTuple_Check(args)) {
		/* ERROR */
		return 0;
	}	
	l = PyTuple_GET_SIZE(args);
	if (l < min) {
		if (name != NULL)
			;/* ERROR */
		else
			;/* ERROR */	
		va_end(vargs);
		return 0;
	}
	if (l > max) {
		if (name != NULL)
			;/* ERROR */
		else
			;/* ERROR */
		va_end(vargs);
		return 0;
	}
	for (i = 0; i < l; i++) {
		o = va_arg(vargs, PyObject **);
		*o = PyTuple_GET_ITEM(args, i);
	}
	va_end(vargs);
	return 1;
}
