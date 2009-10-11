#include "Python.h"

#define NEW_STYLE_NUMBER(o) PyType_HasFeature((o)->ob_type, \
				Py_TPFLAGS_CHECKTYPES)



int
PyObject_SetItem(PyObject *o, PyObject *key, PyObject *value)
{
        PyMappingMethods *m;

        if (o == NULL || key == NULL || value == NULL) {
//               null_error();
                return -1;
        }
        m = o->ob_type->tp_as_mapping;
        if (m && m->mp_ass_subscript)
                return m->mp_ass_subscript(o, key, value);

        if (o->ob_type->tp_as_sequence) {
//                if (PyInt_Check(key))
//                        return PySequence_SetItem(o, PyInt_AsLong(key), value);
                        return PySequence_SetItem(o, PyInt_AS_LONG(key), value);
//                else if (PyLong_Check(key)) {
//                        long key_value = PyLong_AsLong(key);
//                        if (key_value == -1 && PyErr_Occurred())
//                                return -1;
//                        return PySequence_SetItem(o, key_value, value);
//                }
//                type_error("sequence index must be integer");
                return -1;
        }

//        type_error("object does not support item assignment");
        return -1;
}


#define NB_SLOT(x) offsetof(PyNumberMethods, x)
#define NB_BINOP(nb_methods, slot) \
		(*(binaryfunc*)(& ((char*)nb_methods)[slot]))


static PyObject *
binary_op1(PyObject *v, PyObject *w, const int op_slot)
{
	PyObject *x;
	binaryfunc slotv = NULL;
	binaryfunc slotw = NULL;

	if (v->ob_type->tp_as_number != NULL && NEW_STYLE_NUMBER(v))
		slotv = NB_BINOP(v->ob_type->tp_as_number, op_slot);
	if (w->ob_type != v->ob_type &&
	    w->ob_type->tp_as_number != NULL && NEW_STYLE_NUMBER(w)) {
		slotw = NB_BINOP(w->ob_type->tp_as_number, op_slot);
		if (slotw == slotv)
			slotw = NULL;
	}
	if (slotv) {
		if (slotw && PyType_IsSubtype(w->ob_type, v->ob_type)) {
			x = slotw(v, w);
			if (x != Py_NotImplemented)
				return x;
			Py_DECREF(x); /* can't do it */
			slotw = NULL;
		}
		x = slotv(v, w);
		if (x != Py_NotImplemented)
			return x;
		Py_DECREF(x); /* can't do it */
	}
	if (slotw) {
		x = slotw(v, w);
		if (x != Py_NotImplemented)
			return x;
		Py_DECREF(x); /* can't do it */
	}

	if (!NEW_STYLE_NUMBER(v) || !NEW_STYLE_NUMBER(w)) {

		int err = PyNumber_CoerceEx(&v, &w);
		if (err < 0) {
			return NULL;
		}
		if (err == 0) {
			PyNumberMethods *mv = v->ob_type->tp_as_number;
			if (mv) {
				binaryfunc slot;
				slot = NB_BINOP(mv, op_slot);
				if (slot) {
					PyObject *x = slot(v, w);
					Py_DECREF(v);
					Py_DECREF(w);
					return x;
				}
			}
			/* CoerceEx incremented the reference counts */
			Py_DECREF(v);
			Py_DECREF(w);
		}
	}
	Py_INCREF(Py_NotImplemented);
	return Py_NotImplemented;
}

static PyObject *
binop_type_error(PyObject *v, PyObject *w, const char *op_name)
{
	/* ERROR */
	printf("binop_type_error\n");
	return NULL;
}

static PyObject *
binary_op(PyObject *v, PyObject *w, const int op_slot, const char *op_name)
{
	PyObject *result = binary_op1(v, w, op_slot);
	if (result == Py_NotImplemented) {
		Py_DECREF(result);
		return binop_type_error(v, w, op_name);
	}
	return result;
}

#define BINARY_FUNC(func, op, op_name) \
    PyObject * \
    func(PyObject *v, PyObject *w) { \
	    return binary_op(v, w, NB_SLOT(op), op_name); \
    }

BINARY_FUNC(PyNumber_And, nb_and, "&")

PyObject *
PyNumber_Remainder(PyObject *v, PyObject *w)
{
	return binary_op(v, w, NB_SLOT(nb_remainder), "%");
}

PyObject *
PyObject_Call(PyObject *func, PyObject *arg, PyObject *kw)
{
        ternaryfunc call;

	if ((call = func->ob_type->tp_call) != NULL) {
		PyObject *result = (*call)(func, arg, kw);
		return result;
	}
	return NULL;
}

int
PySequence_SetItem(PyObject *s, int i, PyObject *o)
{
        PySequenceMethods *m;

        if (s == NULL) {
//                null_error();
                return -1;
        }

        m = s->ob_type->tp_as_sequence;
        if (m && m->sq_ass_item) {
                if (i < 0) {
                        if (m->sq_length) {
                                int l = (*m->sq_length)(s);
                                if (l < 0)
                                        return -1;
                                i += l;
                        }
                }
                return m->sq_ass_item(s, i, o);
        }

//        type_error("object doesn't support item assignment");
        return -1;
}

int
PySequence_SetSlice(PyObject *s, int i1, int i2, PyObject *o)
{
        PySequenceMethods *m;
        PyMappingMethods *mp;

        if (s == NULL) {
//                null_error();
                return -1;
        }

        m = s->ob_type->tp_as_sequence;
        if (m && m->sq_ass_slice) {
                if (i1 < 0 || i2 < 0) {
                        if (m->sq_length) {
                                int l = (*m->sq_length)(s);
                                if (l < 0)
                                        return -1;
                                if (i1 < 0)
                                        i1 += l;
                                if (i2 < 0)
                                        i2 += l;
                        }
                }
                return m->sq_ass_slice(s, i1, i2, o);
//        } else if ((mp = s->ob_type->tp_as_mapping) && mp->mp_ass_subscript) {
//               int res;
//                PyObject *slice = sliceobj_from_intint(i1, i2);
//              if (!slice)
//                      return -1;
//              res = mp->mp_ass_subscript(s, slice, o);
//              Py_DECREF(slice);
//              return res;
        }

//        type_error("object doesn't support slice assignment");
        return -1;
}

PyObject *
PyObject_GetItem(PyObject *o, PyObject *key)
{
	PyMappingMethods *m;
	PySequenceMethods *s;

	if (o == NULL || key == NULL)
		Py_FatalError("null_error");

	m = o->ob_type->tp_as_mapping;
	if (m && m->mp_subscript) {
		return m->mp_subscript(o, key);
	}

	s = o->ob_type->tp_as_sequence;
	if (s && s->sq_item) {
		return s->sq_item(o, key);
	}

	/* @@@ */
	Py_FatalError("unsubscriptable object");
}

