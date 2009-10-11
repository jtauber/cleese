#include "Python.h"

extern unsigned char M___main__[];

static struct _frozen frozenModules[] = {
	{"__main__", M___main__, 93}, 
	{0, 0, 0} /* sentinel */
};

void Py_Main()
{
	LOG("> Py_Main\n");

	PyImport_FrozenModules = frozenModules;

	Py_Initialize();

	PyImport_ImportFrozenModule("__main__");

	Py_Finalize();

	LOG("< Py_Main\n");
}
