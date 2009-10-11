
/* Class object implementation */

#include "Python.h"
#include "structmember.h"

/* Forward */
static PyObject *class_lookup(PyClassObject *, PyObject *,
			      PyClassObject **);

static PyObject *getattrstr, *setattrstr, *delattrstr;

static PyObject *
class_lookup(PyClassObject *cp, PyObject *name, PyClassObject **pclass)
{
	int i, n;
	PyObject *value = PyDict_GetItem(cp->cl_dict, name);
	if (value != NULL) {
		*pclass = cp;
		return value;
	}
	n = PyTuple_Size(cp->cl_bases);
	for (i = 0; i < n; i++) {
		/* XXX What if one of the bases is not a class? */
		PyObject *v = class_lookup(
			(PyClassObject *)
			PyTuple_GetItem(cp->cl_bases, i), name, pclass);
		if (v != NULL)
			return v;
	}
	return NULL;
}

PyObject *
PyClass_New(PyObject *bases, PyObject *dict, PyObject *name)
     /* bases is NULL or tuple of classobjects! */
{
	PyClassObject *op, *dummy;
	static PyObject *docstr, *modstr, *namestr;
	if (docstr == NULL) {
		docstr= PyString_InternFromString("__doc__");
		if (docstr == NULL)
			return NULL;
	}
	if (modstr == NULL) {
		modstr= PyString_InternFromString("__module__");
		if (modstr == NULL)
			return NULL;
	}
	if (namestr == NULL) {
		namestr= PyString_InternFromString("__name__");
		if (namestr == NULL)
			return NULL;
	}
	if (name == NULL || !PyString_Check(name)) {
		PyErr_SetString(PyExc_TypeError,
				"PyClass_New: name must be a string");
		return NULL;
	}
	if (dict == NULL || !PyDict_Check(dict)) {
		PyErr_SetString(PyExc_TypeError,
				"PyClass_New: dict must be a dictionary");
		return NULL;
	}
	if (PyDict_GetItem(dict, docstr) == NULL) {
		if (PyDict_SetItem(dict, docstr, Py_None) < 0)
			return NULL;
	}
	if (PyDict_GetItem(dict, modstr) == NULL) {
		PyObject *globals = PyEval_GetGlobals();
		if (globals != NULL) {
			PyObject *modname = PyDict_GetItem(globals, namestr);
			if (modname != NULL) {
				if (PyDict_SetItem(dict, modstr, modname) < 0)
					return NULL;
			}
		}
	}
	if (bases == NULL) {
		bases = PyTuple_New(0);
		if (bases == NULL)
			return NULL;
	}
	else {
		int i, n;
		PyObject *base;
		if (!PyTuple_Check(bases)) {
			PyErr_SetString(PyExc_TypeError,
					"PyClass_New: bases must be a tuple");
			return NULL;
		}
		n = PyTuple_Size(bases);
		for (i = 0; i < n; i++) {
			base = PyTuple_GET_ITEM(bases, i);
			if (!PyClass_Check(base)) {
				if (PyCallable_Check(
					(PyObject *) base->ob_type))
					return PyObject_CallFunction(
						(PyObject *) base->ob_type,
						"OOO",
						name,
						bases,
						dict);
				PyErr_SetString(PyExc_TypeError,
					"PyClass_New: base must be a class");
				return NULL;
			}
		}
		Py_INCREF(bases);
	}
	op = PyObject_GC_New(PyClassObject, &PyClass_Type);
	if (op == NULL) {
		Py_DECREF(bases);
		return NULL;
	}
	op->cl_bases = bases;
	Py_INCREF(dict);
	op->cl_dict = dict;
	Py_XINCREF(name);
	op->cl_name = name;
	if (getattrstr == NULL) {
		getattrstr = PyString_InternFromString("__getattr__");
		setattrstr = PyString_InternFromString("__setattr__");
		delattrstr = PyString_InternFromString("__delattr__");
	}
	op->cl_getattr = class_lookup(op, getattrstr, &dummy);
	op->cl_setattr = class_lookup(op, setattrstr, &dummy);
	op->cl_delattr = class_lookup(op, delattrstr, &dummy);
	Py_XINCREF(op->cl_getattr);
	Py_XINCREF(op->cl_setattr);
	Py_XINCREF(op->cl_delattr);
	_PyObject_GC_TRACK(op);
	return (PyObject *) op;
}

PyTypeObject PyClass_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"classobj",
	sizeof(PyClassObject),
	0,
	0, //(destructor)class_dealloc,		/* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	0, //(reprfunc)class_repr,			/* tp_repr */
	0,					/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0,					/* tp_hash */
	0, //PyInstance_New,				/* tp_call */
	0, //(reprfunc)class_str,			/* tp_str */
	0, //(getattrofunc)class_getattr,		/* tp_getattro */
	0, //(setattrofunc)class_setattr,		/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,/* tp_flags */
	0, //class_doc,				/* tp_doc */
	0, //(traverseproc)class_traverse,		/* tp_traverse */
 	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	0,					/* tp_methods */
	0,					/* tp_members */
	0,					/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	0,					/* tp_init */
	0,					/* tp_alloc */
	0, //class_new,				/* tp_new */
};


int
PyClass_IsSubclass(PyObject *class, PyObject *base)
{
	int i, n;
	PyClassObject *cp;
	if (class == base)
		return 1;
	if (PyTuple_Check(base)) {
		n = PyTuple_GET_SIZE(base);
		for (i = 0; i < n; i++) {
			if (PyClass_IsSubclass(class, PyTuple_GET_ITEM(base, i)))
				return 1;
		}
		return 0;
	}
	if (class == NULL || !PyClass_Check(class))
		return 0;
	cp = (PyClassObject *)class;
	n = PyTuple_Size(cp->cl_bases);
	for (i = 0; i < n; i++) {
		if (PyClass_IsSubclass(PyTuple_GetItem(cp->cl_bases, i), base))
			return 1;
	}
	return 0;
}

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

PyTypeObject PyInstance_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"instance",
	sizeof(PyInstanceObject),
	0,
	0, //(destructor)instance_dealloc,		/* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0, //instance_compare,			/* tp_compare */
	0, //(reprfunc)instance_repr,		/* tp_repr */
	0, //&instance_as_number,			/* tp_as_number */
	0, //&instance_as_sequence,			/* tp_as_sequence */
	0, //&instance_as_mapping,			/* tp_as_mapping */
	0, //(hashfunc)instance_hash,		/* tp_hash */
	0, //instance_call,				/* tp_call */
	0, //(reprfunc)instance_str,			/* tp_str */
	0, //(getattrofunc)instance_getattr,		/* tp_getattro */
	0, //(setattrofunc)instance_setattr,		/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_CHECKTYPES,/*tp_flags*/
	0, //instance_doc,				/* tp_doc */
	0, //(traverseproc)instance_traverse,	/* tp_traverse */
	0,					/* tp_clear */
	0, //instance_richcompare,			/* tp_richcompare */
 	offsetof(PyInstanceObject, in_weakreflist), /* tp_weaklistoffset */
	0, //(getiterfunc)instance_getiter,		/* tp_iter */
	0, //(iternextfunc)instance_iternext,	/* tp_iternext */
	0,					/* tp_methods */
	0,					/* tp_members */
	0,					/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */
	0,					/* tp_dictoffset */
	0,					/* tp_init */
	0,					/* tp_alloc */
	0, //instance_new,				/* tp_new */
};

/* Instance method objects are used for two purposes:
   (a) as bound instance methods (returned by instancename.methodname)
   (b) as unbound methods (returned by ClassName.methodname)
   In case (b), im_self is NULL
*/

static PyMethodObject *free_list;

PyObject *
PyMethod_New(PyObject *func, PyObject *self, PyObject *class)
{
	register PyMethodObject *im;
	if (!PyCallable_Check(func)) {
		PyErr_BadInternalCall();
		return NULL;
	}
	im = free_list;
	if (im != NULL) {
		free_list = (PyMethodObject *)(im->im_self);
		PyObject_INIT(im, &PyMethod_Type);
	}
	else {
		im = PyObject_GC_New(PyMethodObject, &PyMethod_Type);
		if (im == NULL)
			return NULL;
	}
	im->im_weakreflist = NULL;
	Py_INCREF(func);
	im->im_func = func;
	Py_XINCREF(self);
	im->im_self = self;
	Py_XINCREF(class);
	im->im_class = class;
	_PyObject_GC_TRACK(im);
	return (PyObject *)im;
}
