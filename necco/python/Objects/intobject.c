#include "Python.h"

#include "ctype.h"

/* Return 1 if exception raised, 0 if caller should retry using longs */
static int
err_ovf(char *msg)
{
	if (PyErr_Warn(PyExc_OverflowWarning, msg) < 0) {
		if (PyErr_ExceptionMatches(PyExc_OverflowWarning))
			PyErr_SetString(PyExc_OverflowError, msg);
		return 1;
	}
	else
		return 0;
}

#define BLOCK_SIZE	1000	/* 1K less typical malloc overhead */
#define BHEAD_SIZE	8	/* Enough for a 64-bit pointer */
#define N_INTOBJECTS	((BLOCK_SIZE - BHEAD_SIZE) / sizeof(PyIntObject))

struct _intblock {
	struct _intblock *next;
	PyIntObject objects[N_INTOBJECTS];
};

typedef struct _intblock PyIntBlock;

static PyIntBlock *block_list = NULL;
static PyIntObject *free_list = NULL;

static PyIntObject *
fill_free_list(void)
{
	PyIntObject *p, *q;
	/* Python's object allocator isn't appropriate for large blocks. */
	p = (PyIntObject *) PyMem_MALLOC(sizeof(PyIntBlock));
	if (p == NULL)
	  return NULL; /* NO MEM ERROR */
	((PyIntBlock *)p)->next = block_list;
	block_list = (PyIntBlock *)p;
	/* Link the int objects together, from rear to front, then return
	   the address of the last int object in the block. */
	p = &((PyIntBlock *)p)->objects[0];
	q = p + N_INTOBJECTS;
	while (--q > p)
		q->ob_type = (struct _typeobject *)(q-1);
	q->ob_type = NULL;
	return p + N_INTOBJECTS - 1;
}

PyObject *
PyInt_FromString(char *s, char **pend, int base)
{
	char *end;
	long x;
	char buffer[256]; /* For errors */
	int warn = 0;

	if ((base != 0 && base < 2) || base > 36) {
		PyErr_SetString(PyExc_ValueError,
				"int() base must be >= 2 and <= 36");
		return NULL;
	}

	while (*s && isspace(Py_CHARMASK(*s)))
		s++;
	errno = 0;
	if (base == 0 && s[0] == '0') {
		x = (long) PyOS_strtoul(s, &end, base);
		if (x < 0)
			warn = 1;
	}
	else
		x = PyOS_strtol(s, &end, base);
	if (end == s || !isalnum(Py_CHARMASK(end[-1])))
		goto bad;
	while (*end && isspace(Py_CHARMASK(*end)))
		end++;
	if (*end != '\0') {
  bad:
		PyOS_snprintf(buffer, sizeof(buffer),
			      "invalid literal for int(): %.200s", s);
		PyErr_SetString(PyExc_ValueError, buffer);
		return NULL;
	}
	else if (errno != 0) {
		if (err_ovf("string/unicode conversion"))
			return NULL;
		return PyLong_FromString(s, pend, base);
	}
	if (warn) {
		if (PyErr_Warn(PyExc_FutureWarning,
			"int('0...', 0): sign will change in Python 2.4") < 0)
			return NULL;
	}
	if (pend)
		*pend = end;
	return PyInt_FromLong(x);
}

PyObject *
PyInt_FromLong(long ival)
{
	register PyIntObject *v;
	/* small int stuff elided */
	if (free_list == NULL) {
		if ((free_list = fill_free_list()) == NULL)
			return NULL;
	}
	/* Inline PyObject_New */
	v = free_list;
	free_list = (PyIntObject *)v->ob_type;
	PyObject_INIT(v, &PyInt_Type);
	v->ob_ival = ival;
	return (PyObject *) v;
}

long
PyInt_AsLong(register PyObject *op)
{
	if (op && PyInt_Check(op))
		return PyInt_AS_LONG((PyIntObject*) op);

	Py_FatalError("PyInt_AsLong not fully supported");
	
	/* this will never be reached */
	return 0;
}

unsigned long
PyInt_AsUnsignedLongMask(register PyObject *op)
{
	PyNumberMethods *nb;
	PyIntObject *io;
	unsigned long val;

	if (op && PyInt_Check(op))
		return PyInt_AS_LONG((PyIntObject*) op);
	if (op && PyLong_Check(op))
		return PyLong_AsUnsignedLongMask(op);

	if (op == NULL || (nb = op->ob_type->tp_as_number) == NULL ||
	    nb->nb_int == NULL) {
		PyErr_SetString(PyExc_TypeError, "an integer is required");
		return -1;
	}

	io = (PyIntObject*) (*nb->nb_int) (op);
	if (io == NULL)
		return -1;
	if (!PyInt_Check(io)) {
		if (PyLong_Check(io)) {
			val = PyLong_AsUnsignedLongMask((PyObject *)io);
			Py_DECREF(io);
			if (PyErr_Occurred())
				return -1;
			return val;
		}
		else
		{
			Py_DECREF(io);
			PyErr_SetString(PyExc_TypeError,
					"nb_int should return int object");
			return -1;
		}
	}

	val = PyInt_AS_LONG(io);
	Py_DECREF(io);

	return val;
}

int
_PyInt_Init(void)
{
	/* could pre-create small integers */
	return 1;
}

void 
PyInt_Fini(void)
{
	/* clean up if small integers pre-created */
}

#define CONVERT_TO_LONG(obj, lng)		\
	if (PyInt_Check(obj)) {			\
		lng = PyInt_AS_LONG(obj);	\
	}					\
	else {					\
		Py_INCREF(Py_NotImplemented);	\
		return Py_NotImplemented;	\
	}

static PyObject *
int_add(PyIntObject *v, PyIntObject *w)
{
	register long a, b, x;
	CONVERT_TO_LONG(v, a);
	CONVERT_TO_LONG(w, b);
	x = a + b;
	if ((x^a) >= 0 || (x^b) >= 0)
		return PyInt_FromLong(x);
	if (err_ovf("integer addition"))
		return NULL;
	return PyLong_Type.tp_as_number->nb_add((PyObject *)v, (PyObject *)w);
}

static PyObject *
int_sub(PyIntObject *v, PyIntObject *w)
{
	register long a, b, x;
	CONVERT_TO_LONG(v, a);
	CONVERT_TO_LONG(w, b);
	x = a - b;
	if ((x^a) >= 0 || (x^~b) >= 0)
		return PyInt_FromLong(x);
	if (err_ovf("integer subtraction"))
		return NULL;
	return PyLong_Type.tp_as_number->nb_subtract((PyObject *)v,
						     (PyObject *)w);
}

/*
Integer overflow checking for * is painful:  Python tried a couple ways, but
they didn't work on all platforms, or failed in endcases (a product of
-sys.maxint-1 has been a particular pain).

Here's another way:

The native long product x*y is either exactly right or *way* off, being
just the last n bits of the true product, where n is the number of bits
in a long (the delivered product is the true product plus i*2**n for
some integer i).

The native double product (double)x * (double)y is subject to three
rounding errors:  on a sizeof(long)==8 box, each cast to double can lose
info, and even on a sizeof(long)==4 box, the multiplication can lose info.
But, unlike the native long product, it's not in *range* trouble:  even
if sizeof(long)==32 (256-bit longs), the product easily fits in the
dynamic range of a double.  So the leading 50 (or so) bits of the double
product are correct.

We check these two ways against each other, and declare victory if they're
approximately the same.  Else, because the native long product is the only
one that can lose catastrophic amounts of information, it's the native long
product that must have overflowed.
*/

static PyObject *
int_mul(PyObject *v, PyObject *w)
{
	long a, b;
	long longprod;			/* a*b in native long arithmetic */
	double doubled_longprod;	/* (double)longprod */
	double doubleprod;		/* (double)a * (double)b */

	CONVERT_TO_LONG(v, a);
	CONVERT_TO_LONG(w, b);
	longprod = a * b;
	doubleprod = (double)a * (double)b;
	doubled_longprod = (double)longprod;

	/* Fast path for normal case:  small multiplicands, and no info
	   is lost in either method. */
	if (doubled_longprod == doubleprod)
		return PyInt_FromLong(longprod);

	/* Somebody somewhere lost info.  Close enough, or way off?  Note
	   that a != 0 and b != 0 (else doubled_longprod == doubleprod == 0).
	   The difference either is or isn't significant compared to the
	   true value (of which doubleprod is a good approximation).
	*/
	{
		const double diff = doubled_longprod - doubleprod;
		const double absdiff = diff >= 0.0 ? diff : -diff;
		const double absprod = doubleprod >= 0.0 ? doubleprod :
							  -doubleprod;
		/* absdiff/absprod <= 1/32 iff
		   32 * absdiff <= absprod -- 5 good bits is "close enough" */
		if (32.0 * absdiff <= absprod)
			return PyInt_FromLong(longprod);
		else if (err_ovf("integer multiplication"))
			return NULL;
		else
			return PyLong_Type.tp_as_number->nb_multiply(v, w);
	}
}

/* Return type of i_divmod */
enum divmod_result {
	DIVMOD_OK,		/* Correct result */
	DIVMOD_OVERFLOW,	/* Overflow, try again using longs */
	DIVMOD_ERROR		/* Exception raised */
};

static enum divmod_result
i_divmod(register long x, register long y,
         long *p_xdivy, long *p_xmody)
{
	long xdivy, xmody;

	if (y == 0) {
		PyErr_SetString(PyExc_ZeroDivisionError,
				"integer division or modulo by zero");
		return DIVMOD_ERROR;
	}
	/* (-sys.maxint-1)/-1 is the only overflow case. */
	if (y == -1 && x < 0 && x == -x) {
		if (err_ovf("integer division"))
			return DIVMOD_ERROR;
		return DIVMOD_OVERFLOW;
	}
	xdivy = x / y;
	xmody = x - xdivy * y;
	/* If the signs of x and y differ, and the remainder is non-0,
	 * C89 doesn't define whether xdivy is now the floor or the
	 * ceiling of the infinitely precise quotient.  We want the floor,
	 * and we have it iff the remainder's sign matches y's.
	 */
	if (xmody && ((y ^ xmody) < 0) /* i.e. and signs differ */) {
		xmody += y;
		--xdivy;
		assert(xmody && ((y ^ xmody) >= 0));
	}
	*p_xdivy = xdivy;
	*p_xmody = xmody;
	return DIVMOD_OK;
}

static PyObject *
int_div(PyIntObject *x, PyIntObject *y)
{
	long xi, yi;
	long d, m;
	CONVERT_TO_LONG(x, xi);
	CONVERT_TO_LONG(y, yi);
	switch (i_divmod(xi, yi, &d, &m)) {
	case DIVMOD_OK:
		return PyInt_FromLong(d);
	case DIVMOD_OVERFLOW:
		return PyLong_Type.tp_as_number->nb_divide((PyObject *)x,
							   (PyObject *)y);
	default:
		return NULL;
	}
}

static PyObject *
int_classic_div(PyIntObject *x, PyIntObject *y)
{
	long xi, yi;
	long d, m;
	CONVERT_TO_LONG(x, xi);
	CONVERT_TO_LONG(y, yi);
	//	if (Py_DivisionWarningFlag &&
	//	    PyErr_Warn(PyExc_DeprecationWarning, "classic int division") < 0)
	//		return NULL;
	switch (i_divmod(xi, yi, &d, &m)) {
	case DIVMOD_OK:
		return PyInt_FromLong(d);
	case DIVMOD_OVERFLOW:
		return PyLong_Type.tp_as_number->nb_divide((PyObject *)x,
							   (PyObject *)y);
	default:
		return NULL;
	}
}

static PyObject *
int_mod(PyIntObject *x, PyIntObject *y)
{
        long xi, yi;
        long d, m;
        CONVERT_TO_LONG(x, xi);
        CONVERT_TO_LONG(y, yi);
        if (i_divmod(xi, yi, &d, &m) < 0)
                return NULL;
        return PyInt_FromLong(m);
}

static PyObject *
int_true_divide(PyObject *v, PyObject *w)
{
	/* If they aren't both ints, give someone else a chance.  In
	   particular, this lets int/long get handled by longs, which
	   underflows to 0 gracefully if the long is too big to convert
	   to float. */
	if (PyInt_Check(v) && PyInt_Check(w))
		return PyFloat_Type.tp_as_number->nb_true_divide(v, w);
	Py_INCREF(Py_NotImplemented);
	return Py_NotImplemented;
}

static int
int_nonzero(PyIntObject *v)
{
	return v->ob_ival != 0;
}

static PyObject *
int_lshift(PyIntObject *v, PyIntObject *w)
{
        register long a, b;
        CONVERT_TO_LONG(v, a);
        CONVERT_TO_LONG(w, b);
        if (b < 0) {
                PyErr_SetString(PyExc_ValueError, "negative shift count");
                return NULL;
        }
        if (a == 0 || b == 0) {
                Py_INCREF(v);
                return (PyObject *) v;
        }
//        if (b >= LONG_BIT) {
//               return PyInt_FromLong(0L);
//        }
        a = (unsigned long)a << b;
        return PyInt_FromLong(a);
}

static PyObject *
int_rshift(PyIntObject *v, PyIntObject *w)
{
        register long a, b;
        CONVERT_TO_LONG(v, a);
        CONVERT_TO_LONG(w, b);
        if (b < 0) {
                PyErr_SetString(PyExc_ValueError, "negative shift count");
                return NULL;
        }
        if (a == 0 || b == 0) {
                Py_INCREF(v);
                return (PyObject *) v;
        }
//        if (b >= LONG_BIT) {
//                if (a < 0)
//                        a = -1;
//                else
//                        a = 0;
//        }
//        else {
//                a = Py_ARITHMETIC_RIGHT_SHIFT(long, a, b);
//        }
/**/	a = a >> b;
        return PyInt_FromLong(a);
}

static PyObject *
int_and(PyIntObject *v, PyIntObject *w)
{
	register long a, b;
	CONVERT_TO_LONG(v, a);
	CONVERT_TO_LONG(w, b);
	return PyInt_FromLong(a & b);
}

static PyObject *
int_or(PyIntObject *v, PyIntObject *w)
{
	register long a, b;
	CONVERT_TO_LONG(v, a);
	CONVERT_TO_LONG(w, b);
	return PyInt_FromLong(a | b);
}

static int
int_print(PyIntObject *v)
{
	printf("%d", v->ob_ival);
	return 0;
}

static PyNumberMethods int_as_number = {
	(binaryfunc)int_add,	/*nb_add*/
	(binaryfunc)int_sub,	/*nb_subtract*/
	(binaryfunc)int_mul,	/*nb_multiply*/
	(binaryfunc)int_classic_div, /*nb_divide*/
	(binaryfunc)int_mod,	/*nb_remainder*/
	0, //(binaryfunc)int_divmod,	/*nb_divmod*/
	0, //(ternaryfunc)int_pow,	/*nb_power*/
	0, //(unaryfunc)int_neg,	/*nb_negative*/
	0, //(unaryfunc)int_pos,	/*nb_positive*/
	0, //(unaryfunc)int_abs,	/*nb_absolute*/
	(inquiry)int_nonzero,	/*nb_nonzero*/
	0, //(unaryfunc)int_invert,	/*nb_invert*/
	(binaryfunc)int_lshift,	/*nb_lshift*/
	(binaryfunc)int_rshift,	/*nb_rshift*/
	(binaryfunc)int_and,	/*nb_and*/
	0, //(binaryfunc)int_xor,	/*nb_xor*/
	(binaryfunc)int_or,	/*nb_or*/
	0, //int_coerce,		/*nb_coerce*/
	0, //(unaryfunc)int_int,	/*nb_int*/
	0, //(unaryfunc)int_long,	/*nb_long*/
	0, //(unaryfunc)int_float,	/*nb_float*/
	0, //(unaryfunc)int_oct,	/*nb_oct*/
	0, //(unaryfunc)int_hex, 	/*nb_hex*/
	0,			/*nb_inplace_add*/
	0,			/*nb_inplace_subtract*/
	0,			/*nb_inplace_multiply*/
	0,			/*nb_inplace_divide*/
	0,			/*nb_inplace_remainder*/
	0,			/*nb_inplace_power*/
	0,			/*nb_inplace_lshift*/
	0,			/*nb_inplace_rshift*/
	0,			/*nb_inplace_and*/
	0,			/*nb_inplace_xor*/
	0,			/*nb_inplace_or*/
	0, //(binaryfunc)int_div,	/* nb_floor_divide */
	0, //int_true_divide,	/* nb_true_divide */
	0,			/* nb_inplace_floor_divide */
	0,			/* nb_inplace_true_divide */
};

PyTypeObject PyInt_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"int",
	sizeof(PyIntObject),
	0,
	0, //(destructor)int_dealloc,		/* tp_dealloc */
	(printfunc)int_print,			/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0, //(cmpfunc)int_compare,			/* tp_compare */
	0, //(reprfunc)int_repr,			/* tp_repr */
	&int_as_number,				/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0, //(hashfunc)int_hash,			/* tp_hash */
	0,					/* tp_call */
	0, //    (reprfunc)int_repr,			/* tp_str */
	PyObject_GenericGetAttr,		/* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES | Py_TPFLAGS_BASETYPE,		/* tp_flags */
	0, //int_doc,				/* tp_doc */
	0,					/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	0, //int_methods,				/* tp_methods */
	0,					/* tp_members */
	0,					/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	0,					/* tp_init */
	0,					/* tp_alloc */
	0, //int_new,				/* tp_new */
	0, //(freefunc)int_free,           		/* tp_free */
};
