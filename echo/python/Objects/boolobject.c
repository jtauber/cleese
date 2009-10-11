#include "Python.h"

/* The type object for bool.  Note that this cannot be subclassed! */

PyTypeObject PyBool_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"bool",
	sizeof(PyIntObject),
	0,
	0,					/* tp_dealloc */
	0, //(printfunc)bool_print,			/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	0, //(reprfunc)bool_repr,			/* tp_repr */
	0, //&bool_as_number,			/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0,					/* tp_hash */
        0,					/* tp_call */
        0, //(reprfunc)bool_repr,			/* tp_str */
	0,					/* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES, /* tp_flags */
	0, //bool_doc,				/* tp_doc */
	0,					/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	0,					/* tp_methods */
	0,					/* tp_members */
	0,					/* tp_getset */
	&PyInt_Type,				/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	0,					/* tp_init */
	0,					/* tp_alloc */
	0, //bool_new,				/* tp_new */
};

/* Named Zero for link-level compatibility */
PyIntObject _Py_ZeroStruct = {
	PyObject_HEAD_INIT(&PyBool_Type)
	0
};

PyIntObject _Py_TrueStruct = {
	PyObject_HEAD_INIT(&PyBool_Type)
	1
};
