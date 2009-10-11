#include "Python.h"

/* Reference to 'warnings' module, to avoid importing it
   on the fly when the import lock may be held.  See 683658/771097
*/
static PyObject *warnings_module = NULL;

/* Returns a borrowed reference to the 'warnings' module, or NULL.
   If the module is returned, it is guaranteed to have been obtained
   without acquiring the import lock
*/
PyObject *PyModule_GetWarningsModule()
{
        PyObject *typ, *val, *tb;
        PyObject *all_modules;
        /* If we managed to get the module at init time, just use it */
        if (warnings_module)
                return warnings_module;
	
	Py_FatalError("warnings not initialized");
	/* ... */
}

static int initialized = 0;

void
Py_Initialize(void)
{
	LOG("> Py_Initialize\n"); {

	PyInterpreterState *interp;
	PyThreadState *tstate;
	PyObject *bimod;
	extern void _Py_ReadyTypes(void);

	if (initialized)
		return;
	initialized = 1;

	interp = PyInterpreterState_New();

	if (interp == NULL)
		Py_FatalError("Py_Initialize: can't make first interpreter");

	tstate = PyThreadState_New(interp);

	if (tstate == NULL)
		Py_FatalError("Py_Initialize: can't make first thread");

	(void) PyThreadState_Swap(tstate);

	_Py_ReadyTypes();

	if (!_PyFrame_Init())
		Py_FatalError("Py_Initialize: can't init frames");

	if (!_PyInt_Init())
		Py_FatalError("Py_Initialize: can't init ints");

	interp->modules = PyDict_New();
	if (interp->modules == NULL)
		Py_FatalError("Py_Initialize: can't make modules dictionary");

	bimod = _PyBuiltin_Init();
	if (bimod == NULL)
		Py_FatalError("Py_Initialize: can't initialize __builtin__");
	interp->builtins = PyModule_GetDict(bimod);
	Py_INCREF(interp->builtins);
	
	LOG("< Py_Initialize\n");
}}

void
Py_Finalize(void)
{
	LOG("> Py_Finalize\n"); {
	PyInterpreterState *interp;
	PyThreadState *tstate;

	initialized = 0;

	tstate = PyThreadState_Get();
	interp = tstate->interp;

	PyInterpreterState_Clear(interp);

	PyThreadState_Swap(NULL);
	PyInterpreterState_Delete(interp);

	PyFrame_Fini();
	PyInt_Fini();
	LOG("< Py_Finalize\n");
}}

void
Py_FatalError(const char *msg)
{
	printf("FATAL ERROR: %s\n", msg);
	while (1) {}
}
