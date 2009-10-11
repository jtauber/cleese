
/* Built-in functions */

#include "Python.h"

static PyObject *
builtin___import__(PyObject *self, PyObject *args)
{
	char *name;
	/* @@@ hack until I get ParseTuple working */
	name = ((PyStringObject *)(((PyTupleObject *)args)->ob_item[0]))->ob_sval;
	
	return PyImport_ImportModuleEx(name);
}

static PyObject *
builtin_ord(PyObject *self, PyObject *args)
{
        PyObject *obj;
        long ord;
        int size;

//        if (!PyArg_ParseTuple(args, "O:ord", &obj))
/**/	if(!PyArg_UnpackTuple(args, "ord", 1,1, &obj))
                return NULL;

        if (PyString_Check(obj)) {
                size = PyString_GET_SIZE(obj);
                if (size == 1) {
                        ord = (long)((unsigned char)*PyString_AS_STRING(obj));
                        return PyInt_FromLong(ord);
                }
//        } else if (PyUnicode_Check(obj)) {
//                size = PyUnicode_GET_SIZE(obj);
//                if (size == 1) {
//                        ord = (long)*PyUnicode_AS_UNICODE(obj);
//                        return PyInt_FromLong(ord);
//                }
//        } else {
//                PyErr_Format(PyExc_TypeError,
//                             "ord() expected string of length 1, but " \
//                             "%.200s found", obj->ob_type->tp_name);
//                return NULL;
        }
//
//        PyErr_Format(PyExc_TypeError,
//                     "ord() expected a character, "
//                     "but string of length %d found",
//                     size);
        return NULL;
}

static char ord_doc[] =
"ord(c) -> integer\n\
\n\
Return the integer ordinal of a one-character string.";

static PyObject *
builtin_len(PyObject *self, PyObject *args)
{
        PyObject *v;
        long res;

//        if (!PyArg_ParseTuple(args, "O:len", &v))
/**/	if(!PyArg_UnpackTuple(args, "len", 1,1, &v))
                return NULL;
        res = PyObject_Size(v);
        if (res < 0 && PyErr_Occurred())
                return NULL;
        return PyInt_FromLong(res);
}

static char len_doc[] =
"len(object) -> integer\n\
\n\
Return the number of items of a sequence or mapping.";

/* Return number of items in range (lo, hi, step), when arguments are
 * PyInt or PyLong objects.  step > 0 required.  Return a value < 0 if
 * & only if the true value is too large to fit in a signed long.
 * Arguments MUST return 1 with either PyInt_Check() or
 * PyLong_Check().  Return -1 when there is an error.
 */
static long
get_len_of_range_longs(PyObject *lo, PyObject *hi, PyObject *step)
{
	/* -------------------------------------------------------------
	Algorithm is equal to that of get_len_of_range(), but it operates
	on PyObjects (which are assumed to be PyLong or PyInt objects).
	---------------------------------------------------------------*/
	long n;
	PyObject *diff = NULL;
	PyObject *one = NULL;
	PyObject *tmp1 = NULL, *tmp2 = NULL, *tmp3 = NULL;
		/* holds sub-expression evaluations */

	/* if (lo >= hi), return length of 0. */
	if (PyObject_Compare(lo, hi) >= 0)
		return 0;

	if ((one = PyLong_FromLong(1L)) == NULL)
		goto Fail;

	if ((tmp1 = PyNumber_Subtract(hi, lo)) == NULL)
		goto Fail;

	if ((diff = PyNumber_Subtract(tmp1, one)) == NULL)
		goto Fail;

	if ((tmp2 = PyNumber_FloorDivide(diff, step)) == NULL)
		goto Fail;

	if ((tmp3 = PyNumber_Add(tmp2, one)) == NULL)
		goto Fail;

	n = PyLong_AsLong(tmp3);
	if (PyErr_Occurred()) {  /* Check for Overflow */
		PyErr_Clear();
		goto Fail;
	}

	Py_DECREF(tmp3);
	Py_DECREF(tmp2);
	Py_DECREF(diff);
	Py_DECREF(tmp1);
	Py_DECREF(one);
	return n;

  Fail:
	Py_XDECREF(tmp3);
	Py_XDECREF(tmp2);
	Py_XDECREF(diff);
	Py_XDECREF(tmp1);
	Py_XDECREF(one);
	return -1;
}

/* An extension of builtin_range() that handles the case when PyLong
 * arguments are given. */
static PyObject *
handle_range_longs(PyObject *self, PyObject *args)
{
	PyObject *ilow;
	PyObject *ihigh = NULL;
	PyObject *istep = NULL;

	PyObject *curnum = NULL;
	PyObject *v = NULL;
	long bign;
	int i, n;
	int cmp_result;

	PyObject *zero = PyLong_FromLong(0);

	if (zero == NULL)
		return NULL;

	if (!PyArg_UnpackTuple(args, "range", 1, 3, &ilow, &ihigh, &istep)) {
		Py_DECREF(zero);
		return NULL;
	}

	/* Figure out which way we were called, supply defaults, and be
	 * sure to incref everything so that the decrefs at the end
	 * are correct.
	 */
	assert(ilow != NULL);
	if (ihigh == NULL) {
		/* only 1 arg -- it's the upper limit */
		ihigh = ilow;
		ilow = NULL;
	}
	assert(ihigh != NULL);
	Py_INCREF(ihigh);

	/* ihigh correct now; do ilow */
	if (ilow == NULL)
		ilow = zero;
	Py_INCREF(ilow);

	/* ilow and ihigh correct now; do istep */
	if (istep == NULL) {
		istep = PyLong_FromLong(1L);
		if (istep == NULL)
			goto Fail;
	}
	else {
		Py_INCREF(istep);
	}

	if (!PyInt_Check(ilow) && !PyLong_Check(ilow)) {
		PyErr_Format(PyExc_TypeError,
			     "range() integer start argument expected, got %s.",
			     ilow->ob_type->tp_name);
		goto Fail;
	}

	if (!PyInt_Check(ihigh) && !PyLong_Check(ihigh)) {
		PyErr_Format(PyExc_TypeError,
			     "range() integer end argument expected, got %s.",
			     ihigh->ob_type->tp_name);
		goto Fail;
	}

	if (!PyInt_Check(istep) && !PyLong_Check(istep)) {
		PyErr_Format(PyExc_TypeError,
			     "range() integer step argument expected, got %s.",
			     istep->ob_type->tp_name);
		goto Fail;
	}

	if (PyObject_Cmp(istep, zero, &cmp_result) == -1)
		goto Fail;
	if (cmp_result == 0) {
		PyErr_SetString(PyExc_ValueError,
				"range() step argument must not be zero");
		goto Fail;
	}

	if (cmp_result > 0)
		bign = get_len_of_range_longs(ilow, ihigh, istep);
	else {
		PyObject *neg_istep = PyNumber_Negative(istep);
		if (neg_istep == NULL)
			goto Fail;
		bign = get_len_of_range_longs(ihigh, ilow, neg_istep);
		Py_DECREF(neg_istep);
	}

	n = (int)bign;
	if (bign < 0 || (long)n != bign) {
		PyErr_SetString(PyExc_OverflowError,
				"range() result has too many items");
		goto Fail;
	}

	v = PyList_New(n);
	if (v == NULL)
		goto Fail;

	curnum = ilow;
	Py_INCREF(curnum);

	for (i = 0; i < n; i++) {
		PyObject *w = PyNumber_Long(curnum);
		PyObject *tmp_num;
		if (w == NULL)
			goto Fail;

		PyList_SET_ITEM(v, i, w);

		tmp_num = PyNumber_Add(curnum, istep);
		if (tmp_num == NULL)
			goto Fail;

		Py_DECREF(curnum);
		curnum = tmp_num;
	}
	Py_DECREF(ilow);
	Py_DECREF(ihigh);
	Py_DECREF(istep);
	Py_DECREF(zero);
	Py_DECREF(curnum);
	return v;

  Fail:
	Py_DECREF(ilow);
	Py_DECREF(ihigh);
	Py_XDECREF(istep);
	Py_DECREF(zero);
	Py_XDECREF(curnum);
	Py_XDECREF(v);
	return NULL;
}

/* Return number of items in range/xrange (lo, hi, step).  step > 0
 * required.  Return a value < 0 if & only if the true value is too
 * large to fit in a signed long.
 */
static long
get_len_of_range(long lo, long hi, long step)
{
	/* -------------------------------------------------------------
	If lo >= hi, the range is empty.
	Else if n values are in the range, the last one is
	lo + (n-1)*step, which must be <= hi-1.  Rearranging,
	n <= (hi - lo - 1)/step + 1, so taking the floor of the RHS gives
	the proper value.  Since lo < hi in this case, hi-lo-1 >= 0, so
	the RHS is non-negative and so truncation is the same as the
	floor.  Letting M be the largest positive long, the worst case
	for the RHS numerator is hi=M, lo=-M-1, and then
	hi-lo-1 = M-(-M-1)-1 = 2*M.  Therefore unsigned long has enough
	precision to compute the RHS exactly.
	---------------------------------------------------------------*/
	long n = 0;
	if (lo < hi) {
		unsigned long uhi = (unsigned long)hi;
		unsigned long ulo = (unsigned long)lo;
		unsigned long diff = uhi - ulo - 1;
		n = (long)(diff / (unsigned long)step + 1);
	}
	return n;
}

static PyObject *
builtin_range(PyObject *self, PyObject *args)
{
	long ilow = 0, ihigh = 0, istep = 1;
	long bign;
	int i, n;

	PyObject *v;

	if (PyTuple_Size(args) <= 1) {
		if (!PyArg_ParseTuple(args,
				"l;range() requires 1-3 int arguments",
				&ihigh)) {
			PyErr_Clear();
			return handle_range_longs(self, args);
		}
	}
	else {
		if (!PyArg_ParseTuple(args,
				"ll|l;range() requires 1-3 int arguments",
				&ilow, &ihigh, &istep)) {
			PyErr_Clear();
			return handle_range_longs(self, args);
		}
	}
	if (istep == 0) {
		PyErr_SetString(PyExc_ValueError,
				"range() step argument must not be zero");
		return NULL;
	}
	if (istep > 0)
		bign = get_len_of_range(ilow, ihigh, istep);
	else
		bign = get_len_of_range(ihigh, ilow, -istep);
	n = (int)bign;
	if (bign < 0 || (long)n != bign) {
		PyErr_SetString(PyExc_OverflowError,
				"range() result has too many items");
		return NULL;
	}
	v = PyList_New(n);
	if (v == NULL)
		return NULL;
	for (i = 0; i < n; i++) {
		PyObject *w = PyInt_FromLong(ilow);
		if (w == NULL) {
			Py_DECREF(v);
			return NULL;
		}
		PyList_SET_ITEM(v, i, w);
		ilow += istep;
	}
	return v;
}

PyDoc_STRVAR(range_doc,
"range([start,] stop[, step]) -> list of integers\n\
\n\
Return a list containing an arithmetic progression of integers.\n\
range(i, j) returns [i, i+1, i+2, ..., j-1]; start (!) defaults to 0.\n\
When step is given, it specifies the increment (or decrement).\n\
For example, range(4) returns [0, 1, 2, 3].  The end point is omitted!\n\
These are exactly the valid indices for a list of 4 elements.");

static PyMethodDef builtin_methods[] = {
 	{"__import__",	builtin___import__, METH_VARARGS, NULL/*doc*/},
        {"len",         builtin_len, 1, len_doc},
        {"ord",         builtin_ord, 1, ord_doc},
 	{"range",	builtin_range,      METH_VARARGS, range_doc},
	{NULL,		NULL},
};

PyObject *
_PyBuiltin_Init(void)
{
	PyObject *mod, *dict;
	mod = Py_InitModule4("__builtin__", builtin_methods,
			     NULL/*doc*/, (PyObject *)NULL,
			     PYTHON_API_VERSION);

	dict = PyModule_GetDict(mod);

	#define SETBUILTIN(NAME, OBJECT) \
	if (PyDict_SetItemString(dict, NAME, (PyObject *)OBJECT) < 0)	\
		return NULL;

	SETBUILTIN("None",		Py_None);

 	SETBUILTIN("enumerate",		&PyEnum_Type);

  	SETBUILTIN("list",		&PyList_Type);

	SETBUILTIN("tuple",		&PyTuple_Type);

	return mod;
}
