#include "Python.h"

#include "marshal.h"
#include "compile.h"
#include "eval.h"

static struct _frozen *
find_frozen(char *name)
{
	struct _frozen *p;

	for (p = PyImport_FrozenModules; ; p++) {
		if (p->name == NULL) {
			LOG("< find_frozen NULL\n");
			return NULL;
		}
		if (strcmp(p->name, name) == 0)
			break;
	}

	return p;
}

PyObject *
PyImport_GetModuleDict(void)
{
	LOG("> PyImport_GetModuleDict\n"); {
	PyInterpreterState *interp = PyThreadState_Get()->interp;
	if (interp->modules == NULL)
		Py_FatalError("PyImport_GetModuleDict: no module dictionary!");
	LOG("< PyImport_GetModuleDict\n");
	return interp->modules;
}}

int
PyImport_ImportFrozenModule(char *name)
{
	printf("Importing\n"); {
	
	struct _frozen *p = find_frozen(name);
	PyObject *co;
	int size;

	if (p == NULL)
		return 0;
	size = p->size;

	printf("Unmarshalling\n");
	
	co = PyMarshal_ReadObjectFromString((char *)p->code, size);

	printf("Executing\n");
	
	PyImport_ExecCodeModule(name, co);

	printf("Done\n");
	
	Py_DECREF(co);

	LOG("< PyImport_ImportFrozenModule\n");
	return 1;
}}

PyObject *
PyImport_ImportModuleEx(char *name)
{
	PyObject *modules;
	PyObject *m;

	PyImport_ImportFrozenModule(name);
	modules = PyImport_GetModuleDict();
	m = PyDict_GetItemString(modules, name);
	if (m == NULL) {
		printf("module %s not properly initialized\n", name);
		return NULL;
	}
	Py_INCREF(m);
	return m;
}

PyObject *
PyImport_AddModule(char *name)
{
	LOG("> PyImport_AddModule\n"); {
	PyObject *modules = PyImport_GetModuleDict();
	PyObject *m;

	if ((m = PyDict_GetItemString(modules, name)) != NULL &&
	    PyModule_Check(m))
		return m;
	m = PyModule_New(name);
	if (m == NULL)
		return NULL;
	if (PyDict_SetItemString(modules, name, m) != 0) {
		Py_DECREF(m);
		return NULL;
	}
	Py_DECREF(m);
	LOG("< PyImport_AddModule\n");
	return m;
}}

PyObject *
PyImport_ExecCodeModule(char *name, PyObject *co)
{
	LOG("> PyImport_ExecCodeModule\n"); {

	PyObject *m, *d, *v;

	m = PyImport_AddModule(name);

	if (m == NULL)
		return NULL;
	d = PyModule_GetDict(m);

	if (PyDict_GetItemString(d, "__builtins__") == NULL) {
		if (PyDict_SetItemString(d, "__builtins__", PyEval_GetBuiltins()) != 0) {
			return NULL;
		}
	}
	
	v = PyEval_EvalCode((PyCodeObject *)co, d, d);

	if (v == NULL)
		return NULL;
	Py_DECREF(v);

	LOG("< PyImport_ExecCodeModule\n");
	return m;
}}
