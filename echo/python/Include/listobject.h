
/* List object interface */

#ifndef Py_LISTOBJECT_H
#define Py_LISTOBJECT_H

typedef struct {
	PyObject_VAR_HEAD
	PyObject **ob_item;
} PyListObject;

PyAPI_DATA(PyTypeObject) PyList_Type;

#define PyList_CheckExact(op) ((op)->ob_type == &PyList_Type)

/* Macro, trading safety for speed */
#define PyList_GET_ITEM(op, i) (((PyListObject *)(op))->ob_item[i])
#define PyList_SET_ITEM(op, i, v) (((PyListObject *)(op))->ob_item[i] = (v))
#define PyList_GET_SIZE(op)    (((PyListObject *)(op))->ob_size)

#endif /* !Py_LISTOBJECT_H */
