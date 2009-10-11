
/* Function object implementation */

#include "Python.h"
#include "compile.h"

PyObject *
PyFunction_New(PyObject *code, PyObject *globals)
{
	PyFunctionObject *op = PyObject_GC_New(PyFunctionObject,
					    &PyFunction_Type);
	if (op != NULL) {
		PyObject *doc;
		PyObject *consts;
		PyObject *module;
		op->func_weakreflist = NULL;
		Py_INCREF(code);
		op->func_code = code;
		Py_INCREF(globals);
		op->func_globals = globals;
		op->func_name = ((PyCodeObject *)code)->co_name;
		Py_INCREF(op->func_name);
		op->func_defaults = NULL; /* No default arguments */
		op->func_closure = NULL;
		consts = ((PyCodeObject *)code)->co_consts;
		if (PyTuple_Size(consts) >= 1) {
			/* @@@ */
			doc = Py_None;
		}
		else
			doc = Py_None;
		Py_INCREF(doc);
		op->func_doc = doc;
		op->func_dict = NULL;
		op->func_module = NULL;

		/* __module__: If module name is in globals, use it.
		   Otherwise, use None.
		*/
		module = PyDict_GetItemString(globals, "__name__");
		if (module) {
		    Py_INCREF(module);
		    op->func_module = module;
		}
	}
	else
		return NULL;
	_PyObject_GC_TRACK(op);
	return (PyObject *)op;
}

int
PyFunction_SetDefaults(PyObject *op, PyObject *defaults)
{
	if (!PyFunction_Check(op)) {
		/* ERROR */
		return -1;
	}
	if (defaults == Py_None)
		defaults = NULL;
	else if (PyTuple_Check(defaults)) {
		Py_XINCREF(defaults);
	}
	else {
		/* ERROR */
		return -1;
	}
	Py_XDECREF(((PyFunctionObject *) op) -> func_defaults);
	((PyFunctionObject *) op) -> func_defaults = defaults;
	return 0;
}

PyTypeObject PyFunction_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"function",
	sizeof(PyFunctionObject),
	0,
	0, //(destructor)func_dealloc,		/* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	0, //(reprfunc)func_repr,			/* tp_repr */
	0,					/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0,					/* tp_hash */
	0, //function_call,				/* tp_call */
	0,					/* tp_str */
	PyObject_GenericGetAttr,		/* tp_getattro */
	0, //PyObject_GenericSetAttr,		/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,/* tp_flags */
	0, //func_doc,				/* tp_doc */
	0, //(traverseproc)func_traverse,		/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
	offsetof(PyFunctionObject, func_weakreflist), /* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	0,					/* tp_methods */
	0, //func_memberlist,			/* tp_members */
	0, //func_getsetlist,			/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
	0, //func_descr_get,				/* tp_descr_get */
	0,					/* tp_descr_set */
	0, //offsetof(PyFunctionObject, func_dict),	/* tp_dictoffset */
	0,					/* tp_init */
	0,					/* tp_alloc */
	0, //func_new,				/* tp_new */
};

/* Static method object */

/* A static method does not receive an implicit first argument.
   To declare a static method, use this idiom:

     class C:
         def f(arg1, arg2, ...): ...
	 f = staticmethod(f)

   It can be called either on the class (e.g. C.f()) or on an instance
   (e.g. C().f()); the instance is ignored except for its class.

   Static methods in Python are similar to those found in Java or C++.
   For a more advanced concept, see class methods above.
*/

typedef struct {
	PyObject_HEAD
	PyObject *sm_callable;
} staticmethod;

static void
sm_dealloc(staticmethod *sm)
{
	_PyObject_GC_UNTRACK((PyObject *)sm);
	Py_XDECREF(sm->sm_callable);
	sm->ob_type->tp_free((PyObject *)sm);
}

static int
sm_traverse(staticmethod *sm, visitproc visit, void *arg)
{
	if (!sm->sm_callable)
		return 0;
	return visit(sm->sm_callable, arg);
}

static int
sm_clear(staticmethod *sm)
{
	Py_XDECREF(sm->sm_callable);
	sm->sm_callable = NULL;

	return 0;
}

static PyObject *
sm_descr_get(PyObject *self, PyObject *obj, PyObject *type)
{
	staticmethod *sm = (staticmethod *)self;

	if (sm->sm_callable == NULL) {
		PyErr_SetString(PyExc_RuntimeError,
				"uninitialized staticmethod object");
		return NULL;
	}
	Py_INCREF(sm->sm_callable);
	return sm->sm_callable;
}

static int
sm_init(PyObject *self, PyObject *args, PyObject *kwds)
{
	staticmethod *sm = (staticmethod *)self;
	PyObject *callable;

	if (!PyArg_UnpackTuple(args, "staticmethod", 1, 1, &callable))
		return -1;
	Py_INCREF(callable);
	sm->sm_callable = callable;
	return 0;
}

PyDoc_STRVAR(staticmethod_doc,
"staticmethod(function) -> method\n\
\n\
Convert a function to be a static method.\n\
\n\
A static method does not receive an implicit first argument.\n\
To declare a static method, use this idiom:\n\
\n\
     class C:\n\
         def f(arg1, arg2, ...): ...\n\
	 f = staticmethod(f)\n\
\n\
It can be called either on the class (e.g. C.f()) or on an instance\n\
(e.g. C().f()).  The instance is ignored except for its class.\n\
\n\
Static methods in Python are similar to those found in Java or C++.\n\
For a more advanced concept, see the classmethod builtin.");

PyTypeObject PyStaticMethod_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"staticmethod",
	sizeof(staticmethod),
	0,
	(destructor)sm_dealloc,			/* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	0,					/* tp_repr */
	0,					/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0,					/* tp_hash */
	0,					/* tp_call */
	0,					/* tp_str */
	PyObject_GenericGetAttr,		/* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
	staticmethod_doc,			/* tp_doc */
	(traverseproc)sm_traverse,		/* tp_traverse */
	(inquiry)sm_clear,			/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	0,					/* tp_methods */
	0,					/* tp_members */
	0,					/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
	sm_descr_get,				/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	sm_init,				/* tp_init */
	PyType_GenericAlloc,			/* tp_alloc */
	PyType_GenericNew,			/* tp_new */
	PyObject_GC_Del,           		/* tp_free */
};

PyObject *
PyStaticMethod_New(PyObject *callable)
{
	staticmethod *sm = (staticmethod *)
		PyType_GenericAlloc(&PyStaticMethod_Type, 0);
	if (sm != NULL) {
		Py_INCREF(callable);
		sm->sm_callable = callable;
	}
	return (PyObject *)sm;
}
