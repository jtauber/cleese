#ifndef Py_DICT_OBJECT_H
#define Py_DICT_OBJECT_H

#define PyDict_MINSIZE 8

typedef struct {
	long me_hash;
	PyObject *me_key;
	PyObject *me_value;
} PyDictEntry;

typedef struct _dictobject PyDictObject;
struct _dictobject {
	PyObject_HEAD
	int ma_fill;
	int ma_used;

	int ma_mask;

	PyDictEntry *ma_table;
	PyDictEntry *(*ma_lookup)(PyDictObject *mp, PyObject *key, long hash);	
	PyDictEntry ma_smalltable[PyDict_MINSIZE];
};

PyAPI_DATA(PyTypeObject) PyDict_Type;

#define PyDict_Check(op) PyObject_TypeCheck(op, &PyDict_Type)

PyAPI_FUNC(PyObject *) PyDict_New(void);
PyAPI_FUNC(PyObject *) PyDict_GetItem(PyObject *mp, PyObject *key);
PyAPI_FUNC(int) PyDict_SetItem(PyObject *mp, PyObject *key, PyObject *item);
PyAPI_FUNC(int) PyDict_DelItem(PyObject *mp, PyObject *key);
PyAPI_FUNC(void) PyDict_Clear(PyObject *mp);
PyAPI_FUNC(int) PyDict_Size(PyObject *mp);
PyAPI_FUNC(PyObject *) PyDict_Copy(PyObject *mp);

PyAPI_FUNC(PyObject *) PyDict_GetItemString(PyObject *dp, const char *key);
PyAPI_FUNC(int) PyDict_SetItemString(PyObject *dp, const char *key, PyObject *item);

#endif /* !Py_DICTOBJECT_H */
