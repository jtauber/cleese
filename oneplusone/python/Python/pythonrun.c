#include "Python.h"

extern void print(const char *);

static int initialized = 0;

void
Py_Initialize(void)
{
	print(" Initializing ");
	
	LOG("> Py_Initialize\n");

	PyInterpreterState *interp;
	PyThreadState *tstate;

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

	LOG("< Py_Initialize\n");
}

void
Py_Finalize(void)
{
	LOG("> Py_Finalize\n");
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
}

void
Py_FatalError(const char *msg)
{
	char x[] = {'F', 'A', 'T', 'A', 'L', ' ', 'E', 'R', 'R', 'O', 'R', ':', ' ', '\0'};
	print(x);
	print(msg);
	while (1) {}
}
