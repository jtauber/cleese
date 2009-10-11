
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
