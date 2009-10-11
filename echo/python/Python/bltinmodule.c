
/* Built-in functions */

#include "Python.h"

static PyObject *
builtin___import__(PyObject *self, PyObject *args)
{
	char *name;
	/* @@@ hack until I get ParseTuple working */
	name = ((PyStringObject *)(((PyTupleObject *)args)->ob_item[0]))->ob_sval;
	
	return PyImport_ImportModuleEx(name);
}

static PyObject *
builtin_inb(PyObject *self, PyObject *args)
{
	PyObject *v = NULL;

	if (!PyArg_UnpackTuple(args, "inb", 1, 1, &v))
		return NULL;
	
	if (!PyInt_CheckExact(v)) {
		/* ERROR */
		return NULL;
	} else {
		unsigned short port = PyInt_AS_LONG(v);
		unsigned char data = in(port);
		PyObject *result = PyInt_FromLong(data);

		return result;
	}
}

static PyObject *
builtin_textbuffer(PyObject *self, PyObject *args)
{
	static PyObject *tb = NULL;

	if(!tb)	{
		tb = PyBuffer_FromReadWriteMemory((void *)0xb8000, 80*25*2);
	}
	Py_INCREF(tb);
	return tb;
}

static PyObject *d, *c, *k = 0;

int python_isr(void *arg)
{
	PyObject *v = NULL;

	switch((int)arg)	{
	case 0: v = c; break;
	case 1: v = k; break;
	}
	if(v)	{
        	Py_DECREF((PyObject *)PyEval_EvalCode(
		 		PyFunction_GET_CODE(v), d, d));
	}
	return 0;
}

static PyObject *
builtin_setvec(PyObject *self, PyObject *args)
{
	PyObject *s = PyThreadState_Get()->interp->modules;
	PyObject *m = PyDict_GetItemString(s, "__main__");

	d = PyModule_GetDict(m);
	c = PyDict_GetItemString(d, "clk_isr");
	k = PyDict_GetItemString(d, "kbd_isr");

	initirqs();

	Py_INCREF(Py_True);
	return Py_True;
}

static PyObject *
builtin_videomode(PyObject *self, PyObject *args)
{
	PyObject *v = NULL;

	if (!PyArg_UnpackTuple(args, "inb", 1, 1, &v))
		return NULL;
	
	if (!PyInt_CheckExact(v))
		return NULL;

	switch(PyInt_AS_LONG(v))	{
	case 0:	set80x25();		break;
	case 1: set640x480x16();	break;
	case 2: set320x200x256();	break;
	}
	Py_INCREF(Py_True);
	return Py_True;
}

static PyMethodDef builtin_methods[] = {
 	{"__import__",	builtin___import__, METH_VARARGS, NULL/*doc*/},
 	{"inb",	        builtin_inb,        METH_VARARGS, NULL/*doc*/},
	{"textbuffer",	builtin_textbuffer, METH_VARARGS, NULL/*doc*/},
	{"setvec",	builtin_setvec,     METH_VARARGS, NULL/*doc*/},
	{"videomode",	builtin_videomode,  METH_VARARGS, NULL/*doc*/},
	{NULL,		NULL},
};

PyObject *
_PyBuiltin_Init(void)
{
	PyObject *mod, *dict;
	mod = Py_InitModule4("__builtin__", builtin_methods,
			     NULL/*doc*/, (PyObject *)NULL,
			     PYTHON_API_VERSION);

	dict = PyModule_GetDict(mod);

	#define SETBUILTIN(NAME, OBJECT) \
	if (PyDict_SetItemString(dict, NAME, (PyObject *)OBJECT) < 0)	\
		return NULL;

	SETBUILTIN("None",		Py_None);
	SETBUILTIN("framebuffer",
		PyBuffer_FromReadWriteMemory((void *)0xa0000, 0x10000));

	return mod;
}
