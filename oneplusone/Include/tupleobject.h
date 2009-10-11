#ifndef Py_TUPLEOBJECT_H
#define Py_TUPLEOBJECT_H

typedef struct {
  PyObject_VAR_HEAD
  PyObject *ob_item[1];
} PyTupleObject;

PyAPI_DATA(PyTypeObject) PyTuple_Type;

#define PyTuple_Check(op) PyObject_TypeCheck(op, &PyTuple_Type)
#define PyTuple_CheckExact(op) ((op)->ob_type == &PyTuple_Type)

PyAPI_FUNC(PyObject *) PyTuple_New(int size);
PyAPI_FUNC(int) PyTuple_Size(PyObject *);
PyAPI_FUNC(PyObject *) PyTuple_GetItem(PyObject *, int);

#define PyTuple_GET_ITEM(op, i) (((PyTupleObject *)(op))->ob_item[i])
#define PyTuple_GET_SIZE(op)    (((PyTupleObject *)(op))->ob_size)
#define PyTuple_SET_ITEM(op, i, v) (((PyTupleObject *)(op))->ob_item[i] = v)

#endif /* !Py_TUPLEOBJECT_H */
