#include "Python.h"

typedef struct {
        PyObject_HEAD
        PyObject *b_base;
        void *b_ptr;
        int b_size;
        int b_readonly;
} PyBufferObject;

static PyObject *d = NULL;

static void init_dict() {
	PyObject *s = PyThreadState_Get()->interp->modules;
	PyObject *m = PyDict_GetItemString(s, "__main__");
	d = PyModule_GetDict(m);
}


extern PyObject *swapstacks();
extern int initstack();

static void *save_area(PyObject *stack)
{
	static void *initspsav;
	PyBufferObject *pbo = (PyBufferObject *)stack;

	return (pbo ? pbo->b_ptr + pbo->b_size - 4
		    : (void *)&initspsav);
}

static PyObject *
_stack_swap(PyObject *obuf, PyObject *oarg)
{
	static PyObject *curstack;
	void *from, *to;

	from = save_area(curstack);
	to   = save_area(obuf);
	curstack = obuf;

	return swapstacks(from, to, oarg);
}

static PyObject *
stack_swap(PyObject *self, PyObject *args)
{
	PyObject *obuf, *oarg;

	obuf = NULL;
	if(!PyArg_UnpackTuple(args, "swap", 1, 2, &oarg, &obuf))
		return NULL;

	if(!PyBuffer_Check(obuf) && obuf)
		return NULL;

	return _stack_swap(obuf, oarg);
}


static PyObject *
stack_init(PyObject *self, PyObject *args)
{
	PyObject *obuf, *ofunc;
	PyBufferObject *pbo;

	if(!d)	{ init_dict(); }

	if(!PyArg_UnpackTuple(args, "init", 2, 2, &obuf, &ofunc))
		return NULL;

	if(!PyBuffer_Check(obuf))
		return NULL;

	pbo = (PyBufferObject *)obuf;
	if(pbo && pbo->b_readonly)
		return NULL;

	*(int *)save_area(obuf) = initstack(
			pbo->b_ptr + pbo->b_size - 4,
			PyFunction_GET_CODE(ofunc), d);

	Py_INCREF(Py_True);
	return Py_True;
}

void
fake_syscall(void *esp)
{
	_stack_swap(NULL, PyInt_FromLong((long)esp));
}

static PyObject *
stack_linux(PyObject *self, PyObject *args)
{
	PyObject *obuf;
	PyBufferObject *pbo;
	int *ptr;

	if(!PyArg_UnpackTuple(args, "linux", 1, 1, &obuf))
		return NULL;

	if(!PyBuffer_Check(obuf))
		return NULL;

	pbo = (PyBufferObject *)obuf;
	if(pbo && pbo->b_readonly)
		return NULL;

	/* this is really platform dependent ---- */
	ptr = (int *)(pbo->b_ptr + pbo->b_size) - 0xA;
	memset(ptr, 0, 0x20);	/* zero registers */
	ptr[8] = pbo->b_ptr;	/* no loader; entry at start of buffer */
	/* -------------------------------------- */

	*(int *)save_area(obuf) = ptr;

	Py_INCREF(Py_True);
	return Py_True;
}

static PyObject *
stack_linaddr(PyObject *self, PyObject *args)
{
	PyObject *obuf, *oaddr;
	PyBufferObject *pbo;
	int addr;

	if(!PyArg_UnpackTuple(args, "linaddr", 2, 2, &obuf, &oaddr))
		return NULL;

	if(!PyBuffer_Check(obuf) ||
           !PyInt_CheckExact(oaddr))
		return NULL;

	pbo  = (PyBufferObject *)obuf;
	addr = PyInt_AS_LONG(oaddr);

	return PyInt_FromLong(*(int *)(addr));
}

static PyMethodDef stack_methods[] = {
	{"swap",	stack_swap,     METH_VARARGS, NULL/*doc*/},
	{"init",	stack_init,	METH_VARARGS, NULL/*doc*/},
	{"linux",	stack_linux,	METH_VARARGS, NULL/*doc*/},
	{"linaddr",	stack_linaddr,	METH_VARARGS, NULL/*doc*/},
	{NULL,		NULL},
};

PyObject *
_Stack_Init(void)
{
	return Py_InitModule4("stack", stack_methods,
			     NULL/*doc*/, (PyObject *)NULL,
			     PYTHON_API_VERSION);
}
