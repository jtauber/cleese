/* List object implementation */

#include "Python.h"

PyTypeObject PyList_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"list",
	sizeof(PyListObject),
	0,
	0, //(destructor)list_dealloc,		/* tp_dealloc */
	0, //(printfunc)list_print,			/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	0, //(reprfunc)list_repr,			/* tp_repr */
	0,					/* tp_as_number */
	0, //&list_as_sequence,			/* tp_as_sequence */
	0, //&list_as_mapping,			/* tp_as_mapping */
	0, //list_nohash,				/* tp_hash */
	0,					/* tp_call */
	0,					/* tp_str */
	PyObject_GenericGetAttr,		/* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
		Py_TPFLAGS_BASETYPE,		/* tp_flags */
 	0, //list_doc,				/* tp_doc */
 	0, //(traverseproc)list_traverse,		/* tp_traverse */
 	0, //(inquiry)list_clear,			/* tp_clear */
	0, //list_richcompare,			/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0, //list_iter,				/* tp_iter */
	0,					/* tp_iternext */
	0, //list_methods,				/* tp_methods */
	0,					/* tp_members */
	0,					/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	0, //(initproc)list_init,			/* tp_init */
	0, //PyType_GenericAlloc,			/* tp_alloc */
	0, //PyType_GenericNew,			/* tp_new */
	0, //PyObject_GC_Del,			/* tp_free */
};
