#include "Python.h"

extern void Cleese_Initialize(), Cleese_Finalize();

int
Py_FrozenMain(void)
{
	Py_Initialize();
	Cleese_Initialize();

	PyImport_ImportFrozenModule("__main__");

	Cleese_Finalize();
	Py_Finalize();

	return 0;
}
