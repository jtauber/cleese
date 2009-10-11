#include "Python.h"
#include "longintrepr.h"
#include "ctype.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

/* Normalize (remove leading zeros from) a long int object.
   Doesn't attempt to free the storage--in most cases, due to the nature
   of the algorithms used, this could save at most be one word anyway. */

static PyLongObject *
long_normalize(register PyLongObject *v)
{
	int j = ABS(v->ob_size);
	register int i = j;

	while (i > 0 && v->ob_digit[i-1] == 0)
		--i;
	if (i != j)
		v->ob_size = (v->ob_size < 0) ? -(i) : i;
	return v;
}

/* Allocate a new long int object with size digits.
   Return NULL and set exception if we run out of memory. */

PyLongObject *
_PyLong_New(int size)
{
	return PyObject_NEW_VAR(PyLongObject, &PyLong_Type, size);
}

PyObject *
_PyLong_Copy(PyLongObject *src)
{
	PyLongObject *result;
	int i;

	assert(src != NULL);
	i = src->ob_size;
	if (i < 0)
		i = -(i);
	result = _PyLong_New(i);
	if (result != NULL) {
		result->ob_size = src->ob_size;
		while (--i >= 0)
			result->ob_digit[i] = src->ob_digit[i];
	}
	return (PyObject *)result;
}

/* Create a new long int object from a C long int */

PyObject *
PyLong_FromLong(long ival)
{
	PyLongObject *v;
	unsigned long t;  /* unsigned so >> doesn't propagate sign bit */
	int ndigits = 0;
	int negative = 0;

	if (ival < 0) {
		ival = -ival;
		negative = 1;
	}

	/* Count the number of Python digits.
	   We used to pick 5 ("big enough for anything"), but that's a
	   waste of time and space given that 5*15 = 75 bits are rarely
	   needed. */
	t = (unsigned long)ival;
	while (t) {
		++ndigits;
		t >>= SHIFT;
	}
	v = _PyLong_New(ndigits);
	if (v != NULL) {
		digit *p = v->ob_digit;
		v->ob_size = negative ? -ndigits : ndigits;
		t = (unsigned long)ival;
		while (t) {
			*p++ = (digit)(t & MASK);
			t >>= SHIFT;
		}
	}
	return (PyObject *)v;
}

/* Get a C long int from a long int object.
   Returns -1 and sets an error condition if overflow occurs. */

long
PyLong_AsLong(PyObject *vv)
{
	/* This version by Tim Peters */
	register PyLongObject *v;
	unsigned long x, prev;
	int i, sign;

	if (vv == NULL || !PyLong_Check(vv)) {
		if (vv != NULL && PyInt_Check(vv))
			return PyInt_AsLong(vv);
		PyErr_BadInternalCall();
		return -1;
	}
	v = (PyLongObject *)vv;
	i = v->ob_size;
	sign = 1;
	x = 0;
	if (i < 0) {
		sign = -1;
		i = -(i);
	}
	while (--i >= 0) {
		prev = x;
		x = (x << SHIFT) + v->ob_digit[i];
		if ((x >> SHIFT) != prev)
			goto overflow;
	}
	/* Haven't lost any bits, but if the sign bit is set we're in
	 * trouble *unless* this is the min negative number.  So,
	 * trouble iff sign bit set && (positive || some bit set other
	 * than the sign bit).
	 */
	if ((long)x < 0 && (sign > 0 || (x << 1) != 0))
		goto overflow;
	return (long)x * sign;

 overflow:
	PyErr_SetString(PyExc_OverflowError,
			"long int too large to convert to int");
	return -1;
}

/* Create a new long (or int) object from a C pointer */

PyObject *
PyLong_FromVoidPtr(void *p)
{
#if SIZEOF_VOID_P <= SIZEOF_LONG
	return PyInt_FromLong((long)p);
#else

#ifndef HAVE_LONG_LONG
#   error "PyLong_FromVoidPtr: sizeof(void*) > sizeof(long), but no long long"
#endif
#if SIZEOF_LONG_LONG < SIZEOF_VOID_P
#   error "PyLong_FromVoidPtr: sizeof(PY_LONG_LONG) < sizeof(void*)"
#endif
	/* optimize null pointers */
	if (p == NULL)
		return PyInt_FromLong(0);
	return PyLong_FromLongLong((PY_LONG_LONG)p);

#endif /* SIZEOF_VOID_P <= SIZEOF_LONG */
}

/* *str points to the first digit in a string of base base digits.  base
 * is a power of 2 (2, 4, 8, 16, or 32).  *str is set to point to the first
 * non-digit (which may be *str!).  A normalized long is returned.
 * The point to this routine is that it takes time linear in the number of
 * string characters.
 */
static PyLongObject *
long_from_binary_base(char **str, int base)
{
	char *p = *str;
	char *start = p;
	int bits_per_char;
	int n;
	PyLongObject *z;
	twodigits accum;
	int bits_in_accum;
	digit *pdigit;

	assert(base >= 2 && base <= 32 && (base & (base - 1)) == 0);
	n = base;
	for (bits_per_char = -1; n; ++bits_per_char)
		n >>= 1;
	/* n <- total # of bits needed, while setting p to end-of-string */
	n = 0;
	for (;;) {
		int k = -1;
		char ch = *p;

		if (ch <= '9')
			k = ch - '0';
		else if (ch >= 'a')
			k = ch - 'a' + 10;
		else if (ch >= 'A')
			k = ch - 'A' + 10;
		if (k < 0 || k >= base)
			break;
		++p;
	}
	*str = p;
	n = (p - start) * bits_per_char;
	if (n / bits_per_char != p - start) {
		PyErr_SetString(PyExc_ValueError,
				"long string too large to convert");
		return NULL;
	}
	/* n <- # of Python digits needed, = ceiling(n/SHIFT). */
	n = (n + SHIFT - 1) / SHIFT;
	z = _PyLong_New(n);
	if (z == NULL)
		return NULL;
	/* Read string from right, and fill in long from left; i.e.,
	 * from least to most significant in both.
	 */
	accum = 0;
	bits_in_accum = 0;
	pdigit = z->ob_digit;
	while (--p >= start) {
		int k;
		char ch = *p;

		if (ch <= '9')
			k = ch - '0';
		else if (ch >= 'a')
			k = ch - 'a' + 10;
		else {
			assert(ch >= 'A');
			k = ch - 'A' + 10;
		}
		assert(k >= 0 && k < base);
		accum |= (twodigits)(k << bits_in_accum);
		bits_in_accum += bits_per_char;
		if (bits_in_accum >= SHIFT) {
			*pdigit++ = (digit)(accum & MASK);
			assert(pdigit - z->ob_digit <= n);
			accum >>= SHIFT;
			bits_in_accum -= SHIFT;
			assert(bits_in_accum < SHIFT);
		}
	}
	if (bits_in_accum) {
		assert(bits_in_accum <= SHIFT);
		*pdigit++ = (digit)accum;
		assert(pdigit - z->ob_digit <= n);
	}
	while (pdigit - z->ob_digit < n)
		*pdigit++ = 0;
	return long_normalize(z);
}

PyObject *
PyLong_FromString(char *str, char **pend, int base)
{
	int sign = 1;
	char *start, *orig_str = str;
	PyLongObject *z;

	if ((base != 0 && base < 2) || base > 36) {
		PyErr_SetString(PyExc_ValueError,
				"long() arg 2 must be >= 2 and <= 36");
		return NULL;
	}
	while (*str != '\0' && isspace(Py_CHARMASK(*str)))
		str++;
	if (*str == '+')
		++str;
	else if (*str == '-') {
		++str;
		sign = -1;
	}
	while (*str != '\0' && isspace(Py_CHARMASK(*str)))
		str++;
	if (base == 0) {
		if (str[0] != '0')
			base = 10;
		else if (str[1] == 'x' || str[1] == 'X')
			base = 16;
		else
			base = 8;
	}
	if (base == 16 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
		str += 2;
	start = str;
	if ((base & (base - 1)) == 0)
		z = long_from_binary_base(&str, base);
	else {
		z = _PyLong_New(0);
		for ( ; z != NULL; ++str) {
			int k = -1;
			PyLongObject *temp;

			if (*str <= '9')
				k = *str - '0';
			else if (*str >= 'a')
				k = *str - 'a' + 10;
			else if (*str >= 'A')
				k = *str - 'A' + 10;
			if (k < 0 || k >= base)
				break;
			temp = muladd1(z, (digit)base, (digit)k);
			Py_DECREF(z);
			z = temp;
		}
	}
	if (z == NULL)
		return NULL;
	if (str == start)
		goto onError;
	if (sign < 0 && z != NULL && z->ob_size != 0)
		z->ob_size = -(z->ob_size);
	if (*str == 'L' || *str == 'l')
		str++;
	while (*str && isspace(Py_CHARMASK(*str)))
		str++;
	if (*str != '\0')
		goto onError;
	if (pend)
		*pend = str;
	return (PyObject *) z;

 onError:
	PyErr_Format(PyExc_ValueError,
		     "invalid literal for long(): %.200s", orig_str);
	Py_XDECREF(z);
	return NULL;
}


PyObject *
PyLong_FromDouble(double dval)
{
  Py_FatalError("PyLong_FromDouble not implemented.");

  /* won't get here */
  return NULL;
}

/* Get a C unsigned long int from a long int object, ignoring the high bits.
   Returns -1 and sets an error condition if an error occurs. */

unsigned long
PyLong_AsUnsignedLongMask(PyObject *vv)
{
	register PyLongObject *v;
	unsigned long x;
	int i, sign;

	if (vv == NULL || !PyLong_Check(vv)) {
		PyErr_BadInternalCall();
		return (unsigned long) -1;
	}
	v = (PyLongObject *)vv;
	i = v->ob_size;
	sign = 1;
	x = 0;
	if (i < 0) {
		sign = -1;
		i = -i;
	}
	while (--i >= 0) {
		x = (x << SHIFT) + v->ob_digit[i];
	}
	return x * sign;
}

/* Multiply by a single digit and add a single digit, ignoring the sign. */

static PyLongObject *
muladd1(PyLongObject *a, wdigit n, wdigit extra)
{
	int size_a = ABS(a->ob_size);
	PyLongObject *z = _PyLong_New(size_a+1);
	twodigits carry = extra;
	int i;

	if (z == NULL)
		return NULL;
	for (i = 0; i < size_a; ++i) {
		carry += (twodigits)a->ob_digit[i] * n;
		z->ob_digit[i] = (digit) (carry & MASK);
		carry >>= SHIFT;
	}
	z->ob_digit[i] = (digit) carry;
	return long_normalize(z);
}

static PyNumberMethods long_as_number = {
  0, //(binaryfunc)	long_add,	/*nb_add*/
  0, //(binaryfunc)	long_sub,	/*nb_subtract*/
  0, //(binaryfunc)	long_mul,	/*nb_multiply*/
  0, //(binaryfunc)	long_classic_div, /*nb_divide*/
  0, //(binaryfunc)	long_mod,	/*nb_remainder*/
  0, //(binaryfunc)	long_divmod,	/*nb_divmod*/
  0, //(ternaryfunc)	long_pow,	/*nb_power*/
  0, //(unaryfunc) 	long_neg,	/*nb_negative*/
  0, //(unaryfunc) 	long_pos,	/*tp_positive*/
  0, //(unaryfunc) 	long_abs,	/*tp_absolute*/
  0, //(inquiry)	long_nonzero,	/*tp_nonzero*/
  0, //(unaryfunc)	long_invert,	/*nb_invert*/
  0, //(binaryfunc)	long_lshift,	/*nb_lshift*/
  0, //(binaryfunc)	long_rshift,	/*nb_rshift*/
  0, //(binaryfunc)	long_and,	/*nb_and*/
  0, //(binaryfunc)	long_xor,	/*nb_xor*/
  0, //(binaryfunc)	long_or,	/*nb_or*/
  0, //(coercion)	long_coerce,	/*nb_coerce*/
  0, //(unaryfunc)	long_int,	/*nb_int*/
  0, //(unaryfunc)	long_long,	/*nb_long*/
  0, //(unaryfunc)	long_float,	/*nb_float*/
  0, //(unaryfunc)	long_oct,	/*nb_oct*/
  0, //(unaryfunc)	long_hex,	/*nb_hex*/
	0,				/* nb_inplace_add */
	0,				/* nb_inplace_subtract */
	0,				/* nb_inplace_multiply */
	0,				/* nb_inplace_divide */
	0,				/* nb_inplace_remainder */
	0,				/* nb_inplace_power */
	0,				/* nb_inplace_lshift */
	0,				/* nb_inplace_rshift */
	0,				/* nb_inplace_and */
	0,				/* nb_inplace_xor */
	0,				/* nb_inplace_or */
  0, //(binaryfunc)long_div,		/* nb_floor_divide */
  0, //long_true_divide,		/* nb_true_divide */
	0,				/* nb_inplace_floor_divide */
	0,				/* nb_inplace_true_divide */
};

PyTypeObject PyLong_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,					/* ob_size */
	"long",					/* tp_name */
	sizeof(PyLongObject) - sizeof(digit),	/* tp_basicsize */
	sizeof(digit),				/* tp_itemsize */
	0, //(destructor)long_dealloc,		/* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0, //(cmpfunc)long_compare,			/* tp_compare */
	0, //(reprfunc)long_repr,			/* tp_repr */
	&long_as_number,			/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0, //(hashfunc)long_hash,			/* tp_hash */
        0,              			/* tp_call */
        0, //(reprfunc)long_str,			/* tp_str */
	0, //PyObject_GenericGetAttr,		/* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES |
		Py_TPFLAGS_BASETYPE,		/* tp_flags */
	0, //long_doc,				/* tp_doc */
	0,					/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	0, //long_methods,				/* tp_methods */
	0,					/* tp_members */
	0,					/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	0,					/* tp_init */
	0,					/* tp_alloc */
	0, //long_new,				/* tp_new */
	0, //PyObject_Del,                           /* tp_free */
};
