
/* Module support implementation */

#include "Python.h"

PyObject *
Py_InitModule4(char *name, PyMethodDef *methods, char *doc,
	       PyObject *passthrough, int module_api_version)
{
	PyObject *m, *d, *v, *n;
	PyMethodDef *ml;
	if ((m = PyImport_AddModule(name)) == NULL)
		return NULL;
	d = PyModule_GetDict(m);
	if (methods != NULL) {
		n = PyString_FromString(name);
		if (n == NULL)
			return NULL;
		for (ml = methods; ml->ml_name != NULL; ml++) {
			v = PyCFunction_NewEx(ml, passthrough, n);
			if (v == NULL)
				return NULL;
			if (PyDict_SetItemString(d, ml->ml_name, v) != 0) {
				Py_DECREF(v);
				return NULL;
			}
			Py_DECREF(v);
		}
	}
	return m;
}

static int
countformat(char *format, int endchar)
{
	int count = 0;
	int level = 0;
	while (level > 0 || *format != endchar) {
		switch (*format) {
		case '\0':
			/* Premature end */
			printf("unmatched paren in format\n");
			return -1;
		case '(':
		case '[':
		case '{':
			if (level == 0)
				count++;
			level++;
			break;
		case ')':
		case ']':
		case '}':
			level--;
			break;
		case '#':
		case '&':
		case ',':
		case ':':
		case ' ':
		case '\t':
			break;
		default:
			if (level == 0)
				count++;
		}
		format++;
	}
	return count;
}

/* forward reference */
static PyObject *do_mkvalue(char**, va_list *);

static PyObject *
do_mktuple(char **p_format, va_list *p_va, int endchar, int n)
{
	PyObject *v;
	int i;
	if (n < 0)
		return NULL;
	if ((v = PyTuple_New(n)) == NULL)
		return NULL;
	for (i = 0; i < n; i++) {
		PyObject *w = do_mkvalue(p_format, p_va);
		if (w == NULL) {
			Py_DECREF(v);
			return NULL;
		}
		PyTuple_SetItem(v, i, w);
	}
	if (v != NULL && **p_format != endchar) {
		Py_DECREF(v);
		v = NULL;
		printf("Unmatched paren in format");
	}
	else if (endchar)
		++*p_format;
	return v;
}

static PyObject *
do_mkvalue(char **p_format, va_list *p_va)
{
	for (;;) {
		switch (*(*p_format)++) {
		case '(':
			return do_mktuple(p_format, p_va, ')',
					  countformat(*p_format, ')'));

//		case '[':
//			return do_mklist(p_format, p_va, ']',
//					 countformat(*p_format, ']'));
//
//		case '{':
//			return do_mkdict(p_format, p_va, '}',
//					 countformat(*p_format, '}'));
//
		case 'b':
		case 'B':
		case 'h':
		case 'i':
			return PyInt_FromLong((long)va_arg(*p_va, int));
			
		case 'H':
			return PyInt_FromLong((long)va_arg(*p_va, unsigned int));

		case 'l':
			return PyInt_FromLong((long)va_arg(*p_va, long));

		case 'k':
			return PyInt_FromLong((long)va_arg(*p_va, unsigned long));

		case 'f':
//		case 'd':
//			return PyFloat_FromDouble(
//				(double)va_arg(*p_va, va_double));

		case 'c':
		{
			char p[1];
			p[0] = va_arg(*p_va, int);
			return PyString_FromStringAndSize(p, 1);
		}

		case 's':
		case 'z':
		{
			PyObject *v;
			char *str = va_arg(*p_va, char *);
			int n;
			if (**p_format == '#') {
				++*p_format;
				n = va_arg(*p_va, int);
			}
			else
				n = -1;
			if (str == NULL) {
				v = Py_None;
				Py_INCREF(v);
			}
			else {
				if (n < 0) {
					size_t m = strlen(str);
					if (m > INT_MAX) {
						printf("string too long for Python string");
						return NULL;
					}
					n = (int)m;
				}
				v = PyString_FromStringAndSize(str, n);
			}
			return v;
		}

		case 'N':
		case 'S':
		case 'O':
		if (**p_format == '&') {
			typedef PyObject *(*converter)(void *);
			converter func = va_arg(*p_va, converter);
			void *arg = va_arg(*p_va, void *);
			++*p_format;
			return (*func)(arg);
		}
		else {
			PyObject *v;
			v = va_arg(*p_va, PyObject *);
			if (v != NULL) {
				if (*(*p_format - 1) != 'N')
					Py_INCREF(v);
			}
			else
				printf("NULL object passed to Py_BuildValue");
			return v;
		}

		case ':':
		case ',':
		case ' ':
		case '\t':
			break;

		default:
			printf("bad format char passed to Py_BuildValue");
			return NULL;

		}
	}
}

PyObject *
Py_VaBuildValue(char *format, va_list va)
{
	char *f = format;
	int n = countformat(f, '\0');
	va_list lva;

	lva = va;

	if (n < 0)
		return NULL;
	if (n == 0) {
		Py_INCREF(Py_None);
		return Py_None;
	}
	if (n == 1)
		return do_mkvalue(&f, &lva);
	return do_mktuple(&f, &lva, '\0', n);
}

PyObject *
Py_BuildValue(char *format, ...)
{
	va_list va;
	PyObject* retval;
	va_start(va, format);
	retval = Py_VaBuildValue(format, va);
	va_end(va);
	return retval;
}

