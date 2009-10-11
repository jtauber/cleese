#include "Python.h"

typedef struct {
        PyObject_HEAD
        PyObject *b_base;
        void *b_ptr;
        int b_size;
        int b_readonly;
} PyBufferObject;

static PyObject *
blit_fill(PyObject *self, PyObject *args)
{
	PyObject *ob, *obw, *ox, *oy, *odx, *ody, *ov;
	PyBufferObject *b;
	int bw, x, y, dx, dy;
	void *off;

	if(!PyArg_UnpackTuple(args, "fill", 7, 7,
			&ob, &obw, &ox, &oy, &odx, &ody, &ov))
		return NULL;

	if(!PyBuffer_Check(ob) ||
	   !PyInt_CheckExact(obw) ||
	   !PyInt_CheckExact(ox) ||
	   !PyInt_CheckExact(oy) ||
	   !PyInt_CheckExact(odx) ||
	   !PyInt_CheckExact(ody))
		return NULL;

	b = (PyBufferObject *)ob;
	if(b->b_readonly)
		return NULL;

	bw = PyInt_AS_LONG(obw);
	x  = PyInt_AS_LONG(ox);
	y  = PyInt_AS_LONG(oy);
	dx = PyInt_AS_LONG(odx);
	dy = PyInt_AS_LONG(ody);

	off = b->b_ptr + (bw * y) + x;

	if(PyInt_CheckExact(ov))	{
		char v = PyInt_AS_LONG(ov);
		int i;

		for(i = y; i < y + dy; ++i)	{
			memset(off,v,dx);
			off += bw;
		}
	} else if(PyString_Check(ov))	{
		PyStringObject *pso = (PyStringObject *)ov;
		void *s = pso->ob_sval;
		int l = pso->ob_size;
		int i;
		void *j;
		for(i = y; i < y + dy; ++i)	{
			for(j = off; j < off + dx; j = j + l)	{
				int n = ((j+l < off+dx) ? l : (off+dx-j));
				memcpy(j, s, n);
			}
			off += bw;
		}
	}

	Py_INCREF(Py_True);
	return Py_True;
}

static PyObject *
blit_paste(PyObject *self, PyObject *args)
{
	PyObject *obd, *obdw, *ox, *oy, *obs, *obsw;
	PyBufferObject *bd;
	PyStringObject *bs;
	int bdw, bsw, x, y, dx, dy;
	void *od, *os;
	int i;

	if(!PyArg_UnpackTuple(args, "fill", 6, 6,
			&obd, &obdw, &ox, &oy, &obs, &obsw))
		return NULL;

	if(!PyBuffer_Check(obd) ||
	   !PyInt_CheckExact(obdw) ||
	   !PyInt_CheckExact(ox) ||
	   !PyInt_CheckExact(oy) ||
	   !PyInt_CheckExact(obsw))
		return NULL;

	bd = (PyBufferObject *)obd;
	if(bd->b_readonly)
		return NULL;

	bdw = PyInt_AS_LONG(obdw);
	x  = PyInt_AS_LONG(ox);
	y  = PyInt_AS_LONG(oy);
	bsw = PyInt_AS_LONG(obsw);
	
	dx = bsw;

	if(PyString_Check(obs))	{
		PyStringObject *bs = (PyStringObject *)obs;
		os = bs->ob_sval;
		dy = bs->ob_size / dx;
	} else if(PyBuffer_Check(obs)) {
		PyBufferObject *bs = (PyBufferObject *)obs;
		os = bs->b_ptr;
		dy = bs->b_size / dx;
	} else
		return NULL;

	od = bd->b_ptr + (bdw * y) + x;
	for(i = 0; i < dy; ++i)	{
		memcpy(od,os,dx);
		os += bsw;
		od += bdw;
	}

	Py_INCREF(Py_True);
	return Py_True;
}

static PyMethodDef blit_methods[] = {
	{"fill",	blit_fill,     METH_VARARGS, NULL/*doc*/},
	{"paste",	blit_paste,     METH_VARARGS, NULL/*doc*/},
	{NULL,		NULL},
};

PyObject *
_Blit_Init(void)
{
	return Py_InitModule4("blit", blit_methods,
			     NULL/*doc*/, (PyObject *)NULL,
			     PYTHON_API_VERSION);
}
