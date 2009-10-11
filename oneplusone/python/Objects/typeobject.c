#include "Python.h"

int
PyType_IsSubtype(PyTypeObject *a, PyTypeObject *b)
{
	if (!(a->tp_flags & Py_TPFLAGS_HAVE_CLASS))
		return b == a || b == &PyBaseObject_Type;

	do {
		if (a == b)
			return 1;
		a = a->tp_base;
	} while (a != NULL);
	return b == &PyBaseObject_Type;
}	

PyTypeObject PyType_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,					/* ob_size */
	"type",					/* tp_name */
	0, //sizeof(PyHeapTypeObject),		/* tp_basicsize */
	0, //sizeof(PyMemberDef),			/* tp_itemsize */
	0, //(destructor)type_dealloc,		/* tp_dealloc */
	0,					/* tp_print */
	0,			 		/* tp_getattr */
	0,					/* tp_setattr */
	0, //type_compare,				/* tp_compare */
	0, //(reprfunc)type_repr,			/* tp_repr */
	0,					/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0, //(hashfunc)_Py_HashPointer,		/* tp_hash */
	0, //(ternaryfunc)type_call,			/* tp_call */
	0,					/* tp_str */
	0, //(getattrofunc)type_getattro,		/* tp_getattro */
	0, //(setattrofunc)type_setattro,		/* tp_setattro */
	0,					/* tp_as_buffer */
	0, //Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_BASETYPE,		/* tp_flags */
	0, //type_doc,				/* tp_doc */
	0, //(traverseproc)type_traverse,		/* tp_traverse */
	0, //(inquiry)type_clear,			/* tp_clear */
	0,					/* tp_richcompare */
	0, //offsetof(PyTypeObject, tp_weaklist),	/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	0, //type_methods,				/* tp_methods */
	0, //type_members,				/* tp_members */
	0, //type_getsets,				/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0, //offsetof(PyTypeObject, tp_dict),	/* tp_dictoffset */
	0,					/* tp_init */
	0,					/* tp_alloc */
	0, //type_new,				/* tp_new */
	0, //PyObject_GC_Del,        		/* tp_free */
	0, //(inquiry)type_is_gc,			/* tp_is_gc */
};

PyTypeObject PyBaseObject_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"object",				/* tp_name */
	sizeof(PyObject),			/* tp_basicsize */
	0,					/* tp_itemsize */
	0, //(destructor)object_dealloc,		/* tp_dealloc */
	0,					/* tp_print */
	0,			 		/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	0, //object_repr,				/* tp_repr */
	0,					/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0, //object_hash,				/* tp_hash */
	0,					/* tp_call */
	0, //object_str,				/* tp_str */
	0, //PyObject_GenericGetAttr,		/* tp_getattro */
	0, //PyObject_GenericSetAttr,		/* tp_setattro */
	0,					/* tp_as_buffer */
	0, //Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, //PyDoc_STR("The most base type"),	/* tp_doc */
	0,					/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	0, //object_methods,				/* tp_methods */
	0,					/* tp_members */
	0, //object_getsets,				/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	0, //object_init,				/* tp_init */
	0, //PyType_GenericAlloc,			/* tp_alloc */
	0, //object_new,				/* tp_new */
	0, //PyObject_Del,           		/* tp_free */
};

static int
half_compare(PyObject *self, PyObject *other)
{
  /* TO DO */
  return 2;
}

int
_PyObject_SlotCompare(PyObject *self, PyObject *other)
{
	int c;

	if (self->ob_type->tp_compare == _PyObject_SlotCompare) {
		c = half_compare(self, other);
		if (c <= 1)
			return c;
	}
	if (other->ob_type->tp_compare == _PyObject_SlotCompare) {
		c = half_compare(other, self);
		if (c < -1)
			return -2;
		if (c <= 1)
			return -c;
	}
	return (void *)self < (void *)other ? -1 :
		(void *)self > (void *)other ? 1 : 0;
}

int
PyType_Ready(PyTypeObject *type)
{
	PyTypeObject *base;
	
	if (type->tp_flags & Py_TPFLAGS_READY) {
		return 0;
	}
	
	type->tp_flags |= Py_TPFLAGS_READYING;
	
	/* ... */
	
	base = type->tp_base;
	if (base != NULL) {
		if (type->tp_as_buffer == NULL)
			type->tp_as_buffer = base->tp_as_buffer;
	}
	
	type->tp_flags =
		(type->tp_flags & ~Py_TPFLAGS_READYING) | Py_TPFLAGS_READY;
	return 0;
}    
