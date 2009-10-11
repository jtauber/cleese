#include "Python.h"


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

static int
int_nonzero(PyIntObject *v)
{
	return v->ob_ival != 0;
}

static PyObject *
int_and(PyIntObject *v, PyIntObject *w)
{
	register long a, b;
	CONVERT_TO_LONG(v, a);
	CONVERT_TO_LONG(w, b);
	return PyInt_FromLong(a & b);
}

static int
int_print(PyIntObject *v)
{
	printf("%d", v->ob_ival);
	return 0;
}

static PyNumberMethods int_as_number = {
	0, //(binaryfunc)int_add,	/*nb_add*/
	0, //(binaryfunc)int_sub,	/*nb_subtract*/
	0, //(binaryfunc)int_mul,	/*nb_multiply*/
	0, //(binaryfunc)int_classic_div, /*nb_divide*/
	0, //(binaryfunc)int_mod,	/*nb_remainder*/
	0, //(binaryfunc)int_divmod,	/*nb_divmod*/
	0, //(ternaryfunc)int_pow,	/*nb_power*/
	0, //(unaryfunc)int_neg,	/*nb_negative*/
	0, //(unaryfunc)int_pos,	/*nb_positive*/
	0, //(unaryfunc)int_abs,	/*nb_absolute*/
	(inquiry)int_nonzero,	/*nb_nonzero*/
	0, //(unaryfunc)int_invert,	/*nb_invert*/
	0, //(binaryfunc)int_lshift,	/*nb_lshift*/
	0, //(binaryfunc)int_rshift,	/*nb_rshift*/
	(binaryfunc)int_and,	/*nb_and*/
	0, //(binaryfunc)int_xor,	/*nb_xor*/
	0, //(binaryfunc)int_or,	/*nb_or*/
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
