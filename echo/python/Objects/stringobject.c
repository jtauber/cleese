#include "Python.h"

#define UCHAR_MAX 255

static PyStringObject *characters[UCHAR_MAX + 1];
static PyStringObject *nullstring;

static PyObject *interned;

int
_PyString_Resize(PyObject **pv, int newsize)
{
	/* TO DO */
	return 0;
}


/* Helpers for formatstring */

static PyObject *
getnextarg(PyObject *args, int arglen, int *p_argidx)
{
	int argidx = *p_argidx;
	if (argidx < arglen) {
		(*p_argidx)++;
		if (arglen < 0)
			return args;
		else
			return PyTuple_GetItem(args, argidx);
	}
	/* ERROR */
	return NULL;
}

/* Format codes
 * F_LJUST	'-'
 * F_SIGN	'+'
 * F_BLANK	' '
 * F_ALT	'#'
 * F_ZERO	'0'
 */
#define F_LJUST (1<<0)
#define F_SIGN	(1<<1)
#define F_BLANK (1<<2)
#define F_ALT	(1<<3)
#define F_ZERO	(1<<4)

/* fmt%(v1,v2,...) is roughly equivalent to sprintf(fmt, v1, v2, ...)

   FORMATBUFLEN is the length of the buffer in which the floats, ints, &
   chars are formatted. XXX This is a magic number. Each formatting
   routine does bounds checking to ensure no overflow, but a better
   solution may be to malloc a buffer of appropriate size for each
   format. For now, the current solution is sufficient.
*/
#define FORMATBUFLEN (size_t)120

PyObject *
PyString_Format(PyObject *format, PyObject *args)
{
	char *fmt, *res;
	int fmtcnt, rescnt, reslen, arglen, argidx;
	int args_owned = 0;
	PyObject *result, *orig_args;
	PyObject *dict = NULL;
	if (format == NULL || !PyString_Check(format) || args == NULL) {
		/* ERROR */
		return NULL;
	}
	orig_args = args;
	fmt = PyString_AS_STRING(format);
	fmtcnt = PyString_GET_SIZE(format);
	reslen = rescnt = fmtcnt + 100;
	result = PyString_FromStringAndSize((char *)NULL, reslen);
	if (result == NULL)
		return NULL;
	res = PyString_AsString(result);
	if (PyTuple_Check(args)) {
		arglen = PyTuple_GET_SIZE(args);
		argidx = 0;
	}
	else {
		arglen = -1;
		argidx = -2;
	}
	if (args->ob_type->tp_as_mapping && !PyTuple_Check(args) &&
	    !PyObject_TypeCheck(args, &PyBaseString_Type))
		dict = args;
	while (--fmtcnt >= 0) {
		if (*fmt != '%') {
			if (--rescnt < 0) {
				rescnt = fmtcnt + 100;
				reslen += rescnt;
				if (_PyString_Resize(&result, reslen) < 0)
					return NULL;
				res = PyString_AS_STRING(result)
					+ reslen - rescnt;
				--rescnt;
			}
			*res++ = *fmt++;
		}
		else {
			/* Got a format specifier */
			int flags = 0;
			int width = -1;
			int prec = -1;
			int c = '\0';
			int fill;
			PyObject *v = NULL;
			PyObject *temp = NULL;
			char *pbuf;
			int sign;
			int len;
			char formatbuf[FORMATBUFLEN];
			     /* For format{float,int,char}() */

			fmt++;
			if (*fmt == '(') {
				/* TO DO */
			}
			while (--fmtcnt >= 0) {
				switch (c = *fmt++) {
				case '-': flags |= F_LJUST; continue;
				case '+': flags |= F_SIGN; continue;
				case ' ': flags |= F_BLANK; continue;
				case '#': flags |= F_ALT; continue;
				case '0': flags |= F_ZERO; continue;
				}
				break;
			}
			/* ... */
			if (fmtcnt >= 0) {
				if (c == 'h' || c == 'l' || c == 'L') {
					if (--fmtcnt >= 0)
						c = *fmt++;
				}
			}
			if (fmtcnt < 0) {
				/* ERROR */
				goto error;
			}
			if (c != '%') {
				v = getnextarg(args, arglen, &argidx);
				if (v == NULL)
					goto error;
			}
			sign = 0;
			fill = ' ';
			switch (c) {
			case '%':
				pbuf = "%";
				len = 1;
				break;
			case 's':
				/* Fall through */
			case 'r':
				if (c == 's')
					temp = PyObject_Str(v);
				else {
					/* TO DO */
					goto error;
				}
				if (temp == NULL)
					goto error;
				if (!PyString_Check(temp)) {
					/* XXX Note: this should never happen,
					   since PyObject_Repr() and
					   PyObject_Str() assure this */
					/* ERROR */
					Py_DECREF(temp);
					goto error;
				}
				pbuf = PyString_AS_STRING(temp);
				len = PyString_GET_SIZE(temp);
				if (prec >= 0 && len > prec)
					len = prec;
				break;
			case 'i':
			case 'd':
			case 'u':
			case 'o':
			case 'x':
			case 'X':
				pbuf = formatbuf;

				/* TO DO */
//				len = formatint(pbuf,
//						sizeof(formatbuf),
//						flags, prec, c, v);
				if (len < 0)
					goto error;
				/* only d conversion is signed */
				sign = c == 'd';
				if (flags & F_ZERO)
					fill = '0';
				break;
			case 'e':
			case 'E':
			case 'f':
			case 'g':
			case 'G':
			case 'c':

				printf("FORMAT NOT SUPPORTED YET\n");
				goto error;
			default:
				/* ERROR */
				goto error;
			}
			if (sign) {
				if (*pbuf == '-' || *pbuf == '+') {
					sign = *pbuf++;
					len--;
				}
				else if (flags & F_SIGN)
					sign = '+';
				else if (flags & F_BLANK)
					sign = ' ';
				else
					sign = 0;
			}
			if (width < len)
				width = len;
			if (rescnt - (sign != 0) < width) {
				reslen -= rescnt;
				rescnt = width + fmtcnt + 100;
				reslen += rescnt;
				if (reslen < 0) {
					Py_DECREF(result);
					/* ERROR No mem*/
					return NULL;
				}
				if (_PyString_Resize(&result, reslen) < 0)
					return NULL;
				res = PyString_AS_STRING(result)
					+ reslen - rescnt;
			}
			if (sign) {
				if (fill != ' ')
					*res++ = sign;
				rescnt--;
				if (width > len)
					width--;
			}
			if ((flags & F_ALT) && (c == 'x' || c == 'X')) {
				if (fill != ' ') {
					*res++ = *pbuf++;
					*res++ = *pbuf++;
				}
				rescnt -= 2;
				width -= 2;
				if (width < 0)
					width = 0;
				len -= 2;
			}
			if (width > len && !(flags & F_LJUST)) {
				do {
					--rescnt;
					*res++ = fill;
				} while (--width > len);
			}
			if (fill == ' ') {
				if (sign)
					*res++ = sign;
				if ((flags & F_ALT) &&
				    (c == 'x' || c == 'X')) {
					*res++ = *pbuf++;
					*res++ = *pbuf++;
				}
			}
			memcpy(res, pbuf, len);
			res += len;
			rescnt -= len;
			while (--width >= len) {
				--rescnt;
				*res++ = ' ';
			}
                        if (dict && (argidx < arglen) && c != '%') {
				/* ERROR */
                                goto error;
                        }
			Py_XDECREF(temp);
		} /* '%' */
	} /* until end */
	if (argidx < arglen && !dict) {
		/* ERROR */
		goto error;
	}
	if (args_owned) {
		Py_DECREF(args);
	}
	_PyString_Resize(&result, reslen - rescnt);
	return result;

 error:
	Py_DECREF(result);
	if (args_owned) {
		Py_DECREF(args);
	}
	return NULL;
}

void
PyString_InternInPlace(PyObject **p)
{
	LOG("> PyString_InternInPlace\n"); {
	register PyStringObject *s = (PyStringObject *)(*p);
	PyObject *t;
	if (s == NULL || !PyString_Check(s))
		Py_FatalError("PyString_InternInPlace: strings only please!");
	if (interned == NULL) {
		interned = PyDict_New();
		if (interned == NULL) {
			return;
		}
	}
	if ((t = PyDict_GetItem(interned, (PyObject *)s)) !=NULL) {
		Py_INCREF(t);
		Py_DECREF(*p);
		*p = t;
		return;
	}
	/* Ensure that only true string objects appear in the intern dict */
	if (!PyString_CheckExact(s)) {
		t = PyString_FromStringAndSize(PyString_AS_STRING(s),
						PyString_GET_SIZE(s));
		if (t == NULL) {
		  /* ERROR */
			return;
		}
	} else {
		t = (PyObject*) s;
		Py_INCREF(t);
	}
	if (PyDict_SetItem(interned, t, t) == 0) {
		((PyObject *)t)->ob_refcnt-=2;
		PyString_CHECK_INTERNED(t) = SSTATE_INTERNED_MORTAL;
		Py_DECREF(*p);
		*p = t;
		return;
	}
	Py_DECREF(t);
}}

PyObject *
PyString_FromStringAndSize(const char *str, int size)
{
	LOG("> PyString_FromStringAndSize\n"); {
	register PyStringObject *op;
	if (size == 0 && (op = nullstring) != NULL) {
		Py_INCREF(op);
	LOG("< PyString_FromStringAndSize\n");
		return (PyObject *)op;
	}
	if (size == 1 && str != NULL &&
	    (op = characters[*str & UCHAR_MAX]) != NULL)
	{
		Py_INCREF(op);
	LOG("< PyString_FromStringAndSize\n");
		return (PyObject *)op;
	}

	/* Inline PyObject_NewVar */
	op = (PyStringObject *)
		PyObject_MALLOC(sizeof(PyStringObject) + size * sizeof(char));
	if (op == NULL)
	  return NULL; /* NO MEM */
	PyObject_INIT_VAR(op, &PyString_Type, size);
	op->ob_shash = -1;
	op->ob_sstate = SSTATE_NOT_INTERNED;
	if (str != NULL)
		memcpy(op->ob_sval, str, size);
	op->ob_sval[size] = '\0';
	/* share short strings */

	if (size == 0) {
		PyObject *t = (PyObject *)op;
		PyString_InternInPlace(&t);
		op = (PyStringObject *)t;
		nullstring = op;
		Py_INCREF(op);
	} else if (size == 1 && str != NULL) {
		PyObject *t = (PyObject *)op;
		PyString_InternInPlace(&t);
		op = (PyStringObject *)t;
		characters[*str & UCHAR_MAX] = op;
		Py_INCREF(op);
	}
	LOG("< PyString_FromStringAndSize\n");
	return (PyObject *) op;
}}

PyObject *
PyString_FromString(const char *str)
{
	register size_t size;
	register PyStringObject *op;

	size = strlen(str);

	if (size == 0 && (op = nullstring) != NULL) {
		Py_INCREF(op);
		return (PyObject *)op;
	}
	if (size == 1 && (op = characters[*str & UCHAR_MAX]) != NULL) {
		Py_INCREF(op);
		return (PyObject *)op;
	}

	op = (PyStringObject *) PyObject_MALLOC(sizeof(PyStringObject) + size * sizeof(char));

	if (op == NULL)
		Py_FatalError("no memory");
	PyObject_INIT_VAR(op, &PyString_Type, size);
	op->ob_shash = -1;
	op->ob_sstate = SSTATE_NOT_INTERNED;
	memcpy(op->ob_sval, str, size+1);
	if (size == 0) {
		PyObject *t = (PyObject *)op;
		PyString_InternInPlace(&t);
		op = (PyStringObject *)t;
		nullstring = op;
		Py_INCREF(op);
	} else if (size == 1) {
		PyObject *t = (PyObject *)op;
		PyString_InternInPlace(&t);
		op = (PyStringObject *)t;
		characters[*str & UCHAR_MAX] = op;
		Py_INCREF(op);
	}

	return (PyObject *) op;
}

static int
string_buffer_getreadbuf(PyStringObject *self, int index, const void **ptr)
{
	if ( index != 0 ) {
		Py_FatalError("ERROR: acessing non-existent string segment");
	}
	*ptr = (void *)self->ob_sval;
	return self->ob_size;
}

static int
string_buffer_getwritebuf(PyStringObject *self, int index, const void **ptr)
{
//        PyErr_SetString(PyExc_TypeError,
//                        "Cannot use string as modifiable buffer");
        return -1;
}

static int
string_buffer_getsegcount(PyStringObject *self, int *lenp)
{
        if ( lenp )
                *lenp = self->ob_size;
        return 1;
}

static int
string_buffer_getcharbuf(PyStringObject *self, int index, const char **ptr)
{
        if ( index != 0 ) {
//                PyErr_SetString(PyExc_SystemError,
//                                "accessing non-existent string segment");
                return -1;
        }
        *ptr = self->ob_sval;
        return self->ob_size;
}


static PyBufferProcs string_as_buffer = {
	(getreadbufferproc)string_buffer_getreadbuf,
        (getwritebufferproc)string_buffer_getwritebuf,
        (getsegcountproc)string_buffer_getsegcount,
        (getcharbufferproc)string_buffer_getcharbuf,
};

/* Methods */

static int
string_print(PyStringObject *op)
{
	printf(op->ob_sval);
	return 0;
}

static long
string_hash(PyStringObject *a)
{
	register int len;
	register unsigned char *p;
	register long x;

	if (a->ob_shash != -1)
		return a->ob_shash;
	len = a->ob_size;
	p = (unsigned char *) a->ob_sval;
	x = *p << 7;
	while (--len >= 0)
		x = (1000003*x) ^ *p++;
	x ^= a->ob_size;
	if (x == -1)
		x = -2;
	a->ob_shash = x;
	return x;
}

static PyObject *
string_item(PyStringObject *a, register int i)
{
	PyObject *v;
	char *pchar;
	if (i < 0 || i >= a->ob_size) {
		Py_FatalError("string index out of range");
		return NULL;
	}
	pchar = a->ob_sval + i;
	v = (PyObject *)characters[*pchar & UCHAR_MAX];
	if (v == NULL)
		v = PyString_FromStringAndSize(pchar, 1);
	else {
		Py_INCREF(v);
	}
	return v;
}

static PyObject*
string_subscript(PyStringObject* self, PyObject* item)
{
	if (PyInt_Check(item)) {
		long i = PyInt_AS_LONG(item);
		if (i < 0)
			i += PyString_GET_SIZE(self);
		return string_item(self,i);
	}
	Py_FatalError("only integer indices supported");
}


static PyObject *
string_mod(PyObject *v, PyObject *w)
{
	if (!PyString_Check(v)) {
		Py_INCREF(Py_NotImplemented);
		return Py_NotImplemented;
	}
	return PyString_Format(v, w);
}

static PyNumberMethods string_as_number = {
	0,			/*nb_add*/
	0,			/*nb_subtract*/
	0,			/*nb_multiply*/
	0, 			/*nb_divide*/
	string_mod,		/*nb_remainder*/
};

static PySequenceMethods string_as_sequence = {
	0, //(inquiry)string_length, /*sq_length*/
	0, //(binaryfunc)string_concat, /*sq_concat*/
	0, //(intargfunc)string_repeat, /*sq_repeat*/
	0, //(intargfunc)string_item, /*sq_item*/
	0, //(intintargfunc)string_slice, /*sq_slice*/
	0,		/*sq_ass_item*/
	0,		/*sq_ass_slice*/
	0, //(objobjproc)string_contains /*sq_contains*/
};

static PyMappingMethods string_as_mapping = {
	0, //(inquiry)string_length,
	(binaryfunc)string_subscript,
	0,
};

PyTypeObject PyBaseString_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"basestring",
	0,
	0,
 	0,			 		/* tp_dealloc */
	0,			 		/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	0,		 			/* tp_repr */
	0,					/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0,		 			/* tp_hash */
	0,					/* tp_call */
	0,					/* tp_str */
	0,					/* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, //basestring_doc,				/* tp_doc */
	0,					/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	0,					/* tp_methods */
	0,					/* tp_members */
	0,					/* tp_getset */
	0, //&PyBaseObject_Type,			/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	0,					/* tp_init */
	0,					/* tp_alloc */
	0, //basestring_new,				/* tp_new */
	0,		                	/* tp_free */
};

PyTypeObject PyString_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"str",
	sizeof(PyStringObject),
	sizeof(char),
 	0, //(destructor)string_dealloc, 		/* tp_dealloc */
	(printfunc)string_print, 		/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	0, //(reprfunc)string_repr, 			/* tp_repr */
	&string_as_number,			/* tp_as_number */
	&string_as_sequence,			/* tp_as_sequence */
	&string_as_mapping,			/* tp_as_mapping */
	(hashfunc)string_hash, 			/* tp_hash */
	0,					/* tp_call */
	0, //(reprfunc)string_str,			/* tp_str */
	PyObject_GenericGetAttr,		/* tp_getattro */
	0,					/* tp_setattro */
	&string_as_buffer,			/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES | Py_TPFLAGS_BASETYPE,		/* tp_flags */
	0, //string_doc,				/* tp_doc */
	0,					/* tp_traverse */
	0,					/* tp_clear */
	0, //(richcmpfunc)string_richcompare,	/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	0, //string_methods,				/* tp_methods */
	0,					/* tp_members */
	0,					/* tp_getset */
	&PyBaseString_Type,			/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	0,					/* tp_init */
	0,					/* tp_alloc */
	0, //string_new,				/* tp_new */
	0, //PyObject_Del,	                	/* tp_free */
};

/*const*/ char *
PyString_AsString(register PyObject *op)
{
//	if (!PyString_Check(op))
//		return string_getbuffer(op);
	return ((PyStringObject *)op) -> ob_sval;
}

PyObject *
PyString_InternFromString(const char *cp)
{
	PyObject *s = PyString_FromString(cp);
	if (s == NULL) {
		return NULL;
	}
	PyString_InternInPlace(&s);
	return s;
}

int
_PyString_Eq(PyObject *o1, PyObject *o2)
{
	PyStringObject *a, *b;
	a = (PyStringObject*)o1;
	b = (PyStringObject*)o2;
	return a->ob_size == b->ob_size
	  && *a->ob_sval == *b->ob_sval
          && memcmp(a->ob_sval, b->ob_sval, a->ob_size) == 0;
}

