#include "Python.h"

PyObject *
PyObject_Init(PyObject *op, PyTypeObject *tp)
{
	LOG("> PyObject_Init\n");
	if (op == NULL)
	  Py_FatalError("out of memory");
	/* Any changes should be reflected in PyObject_INIT (objimpl.h) */
	op->ob_type = tp;
	_Py_NewReference(op);
	LOG("< PyObject_Init\n");
	return op;
}

#define RICHCOMPARE(t) (PyType_HasFeature((t), Py_TPFLAGS_HAVE_RICHCOMPARE) \
                         ? (t)->tp_richcompare : NULL)

#define NESTING_LIMIT 20

static int
internal_print(PyObject *op)
{
  return (*op->ob_type->tp_print)(op);
}

int
PyObject_Print(PyObject *op)
{
  return internal_print(op);
}

long
PyObject_Hash(PyObject *v)
{
	PyTypeObject *tp = v->ob_type;
	if (tp->tp_hash != NULL)
		return (*tp->tp_hash)(v);
	/* TO DO */
	return -1;
}

/* ... */

static PyTypeObject PyNone_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "NoneType",
  0,
  0,
  0, //(destructor)none_dealloc,	     /*tp_dealloc*/ /*never called*/
  0,		/*tp_print*/
  0,		/*tp_getattr*/
  0,		/*tp_setattr*/
  0,		/*tp_compare*/
  0, //(reprfunc)none_repr, /*tp_repr*/
  0,		/*tp_as_number*/
  0,		/*tp_as_sequence*/
  0,		/*tp_as_mapping*/
  0,		/*tp_hash */
};

PyObject _Py_NoneStruct = {
  PyObject_HEAD_INIT(&PyNone_Type)
};

static PyTypeObject PyNotImplemented_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "NotImplementedType",
  0,
  0,
  0, //(destructor)none_dealloc,	     /*tp_dealloc*/ /*never called*/
  0,		/*tp_print*/
  0,		/*tp_getattr*/
  0,		/*tp_setattr*/
  0,		/*tp_compare*/
  0, //(reprfunc)NotImplemented_repr, /*tp_repr*/
  0,		/*tp_as_number*/
  0,		/*tp_as_sequence*/
  0,		/*tp_as_mapping*/
  0,		/*tp_hash */
};

PyObject _Py_NotImplementedStruct = {
	PyObject_HEAD_INIT(&PyNotImplemented_Type)
};

void
_Py_ReadyTypes(void)
{
	if (PyType_Ready(&PyType_Type) < 0)
		Py_FatalError("Can't initialize 'type'");

	if (PyType_Ready(&PyString_Type) < 0)
		Py_FatalError("Can't initialize 'str'");

	if (PyType_Ready(&PyNone_Type) < 0)
		Py_FatalError("Can't initialize type(None)");

	if (PyType_Ready(&PyNotImplemented_Type) < 0)
		Py_FatalError("Can't initialize type(NotImplemented)");
}
