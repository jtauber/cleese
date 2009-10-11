
/* Class object implementation */

#include "Python.h"

PyTypeObject PyMethod_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"instancemethod",
	sizeof(PyMethodObject),
	0,
	0, //(destructor)instancemethod_dealloc,	/* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0, //(cmpfunc)instancemethod_compare,	/* tp_compare */
	0, //(reprfunc)instancemethod_repr,		/* tp_repr */
	0,					/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0, //(hashfunc)instancemethod_hash,		/* tp_hash */
	0, //instancemethod_call,			/* tp_call */
	0,					/* tp_str */
	0, //(getattrofunc)instancemethod_getattro,	/* tp_getattro */
	0, //PyObject_GenericSetAttr,		/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,/* tp_flags */
	0, //instancemethod_doc,			/* tp_doc */
	0, //(traverseproc)instancemethod_traverse,	/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
 	offsetof(PyMethodObject, im_weakreflist), /* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	0,					/* tp_methods */
	0, //instancemethod_memberlist,		/* tp_members */
	0,					/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
	0, //instancemethod_descr_get,		/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	0,					/* tp_init */
	0,					/* tp_alloc */
	0, //instancemethod_new,			/* tp_new */
};
