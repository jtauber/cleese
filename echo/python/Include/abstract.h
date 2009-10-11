#ifndef Py_ABSTRACTOBJECT_H
#define Py_ABSTRACTOBJECT_H

     PyAPI_FUNC(PyObject *) PyNumber_Remainder(PyObject *o1, PyObject *o2);

       /*
	 Returns the remainder of dividing o1 by o2, or null on
	 failure.  This is the equivalent of the Python expression:
	 o1%o2.
       */

     PyAPI_FUNC(PyObject *) PyNumber_And(PyObject *o1, PyObject *o2);

       /*
	 Returns the result of bitwise and of o1 and o2 on success, or
	 NULL on failure. This is the equivalent of the Python
	 expression: o1&o2.
       */

     PyAPI_FUNC(PyObject *) PyObject_GetItem(PyObject *o, PyObject *key);

       /*
	 Return element of o corresponding to the object, key, or NULL
	 on failure. This is the equivalent of the Python expression:
	 o[key].
       */

     PyAPI_FUNC(PyObject *) PyObject_Call(PyObject *callable_object,
					 PyObject *args, PyObject *kw);

       /*
	 Call a callable Python object, callable_object, with
	 arguments and keywords arguments.  The 'args' argument can not be
	 NULL, but the 'kw' argument can be NULL.
       */

#endif /* Py_ABSTRACTOBJECT_H */
