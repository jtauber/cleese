#include "Python.h"
#include "compile.h"
#include "marshal.h"

#define TYPE_NONE    'N'
#define TYPE_INT     'i'
#define TYPE_STRING  's'
#define TYPE_TUPLE   '('
#define TYPE_CODE    'c'

typedef struct {
	PyObject *str;
	char *ptr;
	char *end;
} RFILE;

#define rs_byte(p) (((p)->ptr != (p)->end) ? (unsigned char)*(p)->ptr++ : EOF)

static int
r_string(char *s, int n, RFILE *p)
{
	if (p->end - p->ptr < n)
		n = p->end - p->ptr;
	memcpy(s, p->ptr, n);
	p->ptr += n;
	LOGF("--- r_string %s\n", s);
	return n;
}

static short
r_short(RFILE *p)
{
	register short x;
	x = rs_byte(p);
	x |= (long)rs_byte(p) << 8;
	LOGF("--- r_short %ld\n", x);
	return x;
}

static long
r_long(RFILE *p)
{
	register long x;
	x = rs_byte(p);
	x |= (long)rs_byte(p) << 8;
	x |= (long)rs_byte(p) << 16;
	x |= (long)rs_byte(p) << 24;
	LOGF("--- r_long %ld\n", x);
	return x;
}

static PyObject *
r_object(RFILE *p)
{
	PyObject *v, *v2;
	long i, n;
	long (*r_kludge)(RFILE *) = r_long;
	extern int frozenVersionInfo[3];

	int type = rs_byte(p);
#ifdef VERBOSE_MARSHAL
	printf("marshal got %x", type);
#endif
	LOGF("- read %c\n", type);

	/* FIXME: freeze.py should rewrite its marshalled codestring
	          to the 2.3 format; for now I'm lazy and doing it here */
	if(frozenVersionInfo[0] == 2 &&
	   frozenVersionInfo[1] == 1)	{
		r_kludge = (long (*)(RFILE *))r_short;
	}

	switch (type) {

	case TYPE_NONE:
	  LOG("-- none\n");
		Py_INCREF(Py_None);
		return Py_None;

	case TYPE_INT:
	  LOG("-- int\n");
		return PyInt_FromLong(r_long(p));

	case TYPE_STRING:
	  LOG("-- string\n");
		n = r_long(p);
		if (n < 0) {
		  /* ERROR */
			return NULL;
		}
		v = PyString_FromStringAndSize((char *)NULL, n);
		if (v != NULL) {
			if (r_string(PyString_AS_STRING(v), (int)n, p) != n) {
				Py_DECREF(v);
				v = NULL;
				/* ERROR */
			}
		}
		return v;

	case TYPE_TUPLE:
	  LOG("-- tuple\n");
		n = r_long(p);
		if (n < 0) {
			/* ERROR */
			printf("TYPE_TUPLE error\n");
			return NULL;
		}
		v = PyTuple_New((int)n);
		if (v == NULL)
			return v;
		for (i = 0; i < n; i++) {
			v2 = r_object(p);
			if ( v2 == NULL ) {
				Py_DECREF(v);
				v = NULL;
				break;
			}
			PyTuple_SET_ITEM(v, (int)i, v2);
		}
		return v;

	case TYPE_CODE:
	  {
		LOG("-- code\n"); {
		int argcount = r_kludge(p);
		int nlocals = r_kludge(p);
		int stacksize = r_kludge(p);
		int flags = r_kludge(p);
		PyObject *code = NULL;
		PyObject *consts = NULL;
		PyObject *names = NULL;
		PyObject *varnames = NULL;
		PyObject *freevars = NULL;
		PyObject *cellvars = NULL;
		PyObject *filename = NULL;
		PyObject *name = NULL;
		int firstlineno = 0;
		PyObject *lnotab = NULL;
		
		code = r_object(p);
		if (code) consts = r_object(p);
		if (consts) names = r_object(p);
		if (names) varnames = r_object(p);
		if (varnames) freevars = r_object(p);
		if (freevars) cellvars = r_object(p);
		if (cellvars) filename = r_object(p);
		if (filename) name = r_object(p);
		if (name) {
			firstlineno = r_kludge(p);
			lnotab = r_object(p);
		}
		v = (PyObject *) PyCode_New(
					    argcount, nlocals, stacksize, flags,
					    code, consts, names, varnames,
					    freevars, cellvars, filename, name,
					    firstlineno, lnotab);

		Py_XDECREF(code);
		Py_XDECREF(consts);
		Py_XDECREF(names);
		Py_XDECREF(varnames);
		Py_XDECREF(freevars);
		Py_XDECREF(cellvars);
		Py_XDECREF(filename);
		Py_XDECREF(name);
		Py_XDECREF(lnotab);

		return v;
	  }}

	default:
		printf("%x", type);
		Py_FatalError("UNKNOWN TYPE MARSHALLING");
		/* will never get here */
		return NULL;
	}
}

PyObject *
PyMarshal_ReadObjectFromString(char *str, int len)
{
	LOG("> PyMarshal_ReadObjectFromString\n"); {
	RFILE rf;
	rf.str = NULL;
	rf.ptr = str;
	rf.end = str + len;

	{ PyObject *o = r_object(&rf);
	LOG("< PyMarshal_ReadObjectFromString\n");
	return o; }
}}

