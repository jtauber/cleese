#include "Python.h"

#define UCHAR_MAX 255

static PyStringObject *characters[UCHAR_MAX + 1];
static PyStringObject *nullstring;

static PyObject *interned;

void
PyString_InternInPlace(PyObject **p)
{
	LOG("> PyString_InternInPlace\n");
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
}

PyObject *
PyString_FromStringAndSize(const char *str, int size)
{
	LOG("> PyString_FromStringAndSize\n");
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
}

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

static PyBufferProcs string_as_buffer = {
	(getreadbufferproc)string_buffer_getreadbuf,
	0, /* TO DO */
	0, /* TO DO */
	0, /* TO DO */
};

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

PyTypeObject PyString_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"str",
	sizeof(PyStringObject),
	sizeof(char),
 	0, //(destructor)string_dealloc, 		/* tp_dealloc */
	0, //(printfunc)string_print, 		/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	0, //(reprfunc)string_repr, 			/* tp_repr */
	0, //&string_as_number,			/* tp_as_number */
	0, //&string_as_sequence,			/* tp_as_sequence */
	0, //&string_as_mapping,			/* tp_as_mapping */
	(hashfunc)string_hash, 			/* tp_hash */
	0,					/* tp_call */
	0, //(reprfunc)string_str,			/* tp_str */
	0, //PyObject_GenericGetAttr,		/* tp_getattro */
	0,					/* tp_setattro */
	&string_as_buffer,			/* tp_as_buffer */
	0, //Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES | Py_TPFLAGS_BASETYPE,		/* tp_flags */
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
	0, //&PyBaseString_Type,			/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	0,					/* tp_init */
	0,					/* tp_alloc */
	0, //string_new,				/* tp_new */
	0, //PyObject_Del,	                	/* tp_free */
};

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
