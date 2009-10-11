
/* Method object implementation */

#include "Python.h"

static PyCFunctionObject *free_list = NULL;

PyObject *
PyCFunction_NewEx(PyMethodDef *ml, PyObject *self, PyObject *module)
{
	PyCFunctionObject *op;
	op = free_list;
	if (op != NULL) {
		free_list = (PyCFunctionObject *)(op->m_self);
		PyObject_INIT(op, &PyCFunction_Type);
	}
	else {
		op = PyObject_GC_New(PyCFunctionObject, &PyCFunction_Type);
		if (op == NULL)
			return NULL;
	}
	op->m_ml = ml;
	Py_XINCREF(self);
	op->m_self = self;
	Py_XINCREF(module);
	op->m_module = module;
	_PyObject_GC_TRACK(op);
	return (PyObject *)op;
}

PyObject *
PyCFunction_Call(PyObject *func, PyObject *arg, PyObject *kw)
{
  //	PyCFunctionObject* f = (PyCFunctionObject*)func;
	PyCFunction meth = PyCFunction_GET_FUNCTION(func);
	PyObject *self = PyCFunction_GET_SELF(func);
	int size;

	switch (PyCFunction_GET_FLAGS(func) & ~(METH_CLASS | METH_STATIC)) {
	case METH_VARARGS:
		if (kw == NULL || PyDict_Size(kw) == 0)
			return (*meth)(self, arg);
		break;
	case METH_VARARGS | METH_KEYWORDS:
	case METH_OLDARGS | METH_KEYWORDS:
		return (*(PyCFunctionWithKeywords)meth)(self, arg, kw);
	case METH_NOARGS:
		if (kw == NULL || PyDict_Size(kw) == 0) {
			size = PyTuple_GET_SIZE(arg);
			if (size == 0)
				return (*meth)(self, NULL);
			/* ERROR */
			return NULL;
		}
		break;
	case METH_O:
		if (kw == NULL || PyDict_Size(kw) == 0) {
			size = PyTuple_GET_SIZE(arg);
			if (size == 1)
				return (*meth)(self, PyTuple_GET_ITEM(arg, 0));
			/* ERROR */
			return NULL;
		}
		break;
	case METH_OLDARGS:
		/* the really old style */
		if (kw == NULL || PyDict_Size(kw) == 0) {
			size = PyTuple_GET_SIZE(arg);
			if (size == 1)
				arg = PyTuple_GET_ITEM(arg, 0);
			else if (size == 0)
				arg = NULL;
			return (*meth)(self, arg);
		}
		break;
	default:
		/* ERROR */
		return NULL;
	}
	/* ERROR */
	return NULL;
}

PyTypeObject PyCFunction_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"builtin_function_or_method",
	sizeof(PyCFunctionObject),
	0,
	0, //(destructor)meth_dealloc, 		/* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0, //(cmpfunc)meth_compare,			/* tp_compare */
	0, //(reprfunc)meth_repr,			/* tp_repr */
	0,					/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0, //(hashfunc)meth_hash,			/* tp_hash */
	PyCFunction_Call,			/* tp_call */
	0,					/* tp_str */
	PyObject_GenericGetAttr,		/* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,/* tp_flags */
 	0,					/* tp_doc */
 	0, //(traverseproc)meth_traverse,		/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	0,					/* tp_methods */
	0, //meth_members,				/* tp_members */
	0, //meth_getsets,				/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
};
