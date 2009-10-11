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
	LOG("> PyImport_ImportFrozenModule\n"); {
	
	struct _frozen *p = find_frozen(name);
	PyObject *co;
	int size;

	if (p == NULL)
		return 0;
	size = p->size;

	co = PyMarshal_ReadObjectFromString((char *)p->code, size);

	PyImport_ExecCodeModule(name, co);

	Py_DECREF(co);

	LOG("< PyImport_ImportFrozenModule\n");
	return 1;
}}

/* Higher-level import emulator which emulates the "import" statement
   more accurately -- it invokes the __import__() function from the
   builtins of the current globals.  This means that the import is
   done using whatever import hooks are installed in the current
   environment, e.g. by "rexec".
   A dummy list ["__doc__"] is passed as the 4th argument so that
   e.g. PyImport_Import(PyString_FromString("win32com.client.gencache"))
   will return <module "gencache"> instead of <module "win32com">. */

PyObject *
PyImport_Import(PyObject *module_name)
{
	static PyObject *silly_list = NULL;
	static PyObject *builtins_str = NULL;
	static PyObject *import_str = NULL;
	PyObject *globals = NULL;
	PyObject *import = NULL;
	PyObject *builtins = NULL;
	PyObject *r = NULL;

	/* Initialize constant string objects */
	if (silly_list == NULL) {
		import_str = PyString_InternFromString("__import__");
		if (import_str == NULL)
			return NULL;
		builtins_str = PyString_InternFromString("__builtins__");
		if (builtins_str == NULL)
			return NULL;
		silly_list = Py_BuildValue("[s]", "__doc__");
		if (silly_list == NULL)
			return NULL;
	}

	/* Get the builtins from current globals */
	globals = PyEval_GetGlobals();
	if (globals != NULL) {
	        Py_INCREF(globals);
		builtins = PyObject_GetItem(globals, builtins_str);
		if (builtins == NULL)
			goto err;
	}
	else {
		/* No globals -- use standard builtins, and fake globals */
		PyErr_Clear();

		builtins = PyImport_ImportModuleEx("__builtin__");

		if (builtins == NULL)
			return NULL;
		globals = Py_BuildValue("{OO}", builtins_str, builtins);
		if (globals == NULL)
			goto err;
	}

	/* Get the __import__ function from the builtins */
	if (PyDict_Check(builtins)) {
		import = PyObject_GetItem(builtins, import_str);
		if (import == NULL)
			PyErr_SetObject(PyExc_KeyError, import_str);
	}
	else
		import = PyObject_GetAttr(builtins, import_str);
	if (import == NULL)
		goto err;

	/* Call the _import__ function with the proper argument list */
	r = PyObject_CallFunction(import, "OOOO",
				  module_name, globals, globals, silly_list);

  err:
	Py_XDECREF(globals);
	Py_XDECREF(builtins);
	Py_XDECREF(import);

	return r;
}

/* Import a module, either built-in, frozen, or external, and return
   its module object WITH INCREMENTED REFERENCE COUNT */

PyObject *
PyImport_ImportModule(char *name)
{
	PyObject *pname;
	PyObject *result;

	pname = PyString_FromString(name);
	if (pname == NULL)
		return NULL;
	result = PyImport_Import(pname);
	Py_DECREF(pname);
	return result;
}

PyObject *
PyImport_ImportModuleEx(char *name)
{
	PyObject *modules = PyImport_GetModuleDict();
	PyObject *m = PyDict_GetItemString(modules, name);

	if (m == NULL)	{
		PyImport_ImportFrozenModule(name);
		m = PyDict_GetItemString(modules, name);
		if (m == NULL) {
			printf("module %s not properly initialized\n", name);
			return NULL;
		}
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
