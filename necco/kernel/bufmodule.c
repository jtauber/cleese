#include "Python.h"

static PyObject *
buf_bss(PyObject *self, PyObject *args)
{
	PyObject *olen;
	int len;

	if(!PyArg_UnpackTuple(args, "bss", 1, 1, &olen))
		return NULL;
	if(!PyInt_CheckExact(olen))
		return NULL;

	len = PyInt_AS_LONG(olen);

	return PyBuffer_FromReadWriteMemory(malloc(len), len);
}

static PyObject *
buf_abs(PyObject *self, PyObject *args)
{
	PyObject *oaddr, *olen;
	int addr, len;

	if(!PyArg_UnpackTuple(args, "abs", 2, 2, &oaddr, &olen))
		return NULL;
	if(!PyInt_CheckExact(oaddr) ||
	   !PyInt_CheckExact(olen))
		return NULL;

	addr = PyInt_AS_LONG(oaddr);
	len  = PyInt_AS_LONG(olen);

	return PyBuffer_FromReadWriteMemory((void *)addr, len);
}


struct _st_struct {
	char *name;
	void *ptr;
	int len;
};

extern struct _st_struct symtab[];

extern void *bootscreen;
struct _st_struct defsyms[] = {
	{"splashscreen", &bootscreen, 0xFA00 },
	{NULL, NULL, 0},
};

static PyObject *
buf_sym(PyObject *self, PyObject *args)
{
	PyObject *oname;
	char *name;
	int i;

	if(!PyArg_UnpackTuple(args, "res", 1, 1, &oname))
		return NULL;
	if(!PyString_Check(oname))
		return NULL;

	name = PyString_AsString(oname);

	for(i = 0;; ++i)	{
		if(!symtab[i].name)	{
			printf("unknown symbol: %s\n", name);
			return NULL;
		}
		if(strcmp(symtab[i].name, name) == 0)
			break;
	}
	return PyBuffer_FromReadWriteMemory(symtab[i].ptr, symtab[i].len);
}

static PyMethodDef buf_methods[] = {
	{"bss",	buf_bss,     METH_VARARGS, NULL/*doc*/},
	{"abs",	buf_abs,     METH_VARARGS, NULL/*doc*/},
	{"sym", buf_sym,     METH_VARARGS, NULL/*doc*/},
	{NULL,		NULL},
};

PyObject *
_Buf_Init(void)
{
	return Py_InitModule4("buf", buf_methods,
			     NULL/*doc*/, (PyObject *)NULL,
			     PYTHON_API_VERSION);
}
