/* 

derived from Modules/python.c but doesn't take command-line arguments
and calls FrozenMain after setting up own frozen module(s)

*/

#include "Python.h"

static struct _frozen frozenModules[] = {
//        {"__main__", <bytes>, <len>}, 
//        {0, 0, 0} /* sentinel */
};

int
main(void) // main(int argc, char **argv)
{ 
	int res; // int i, res;

	PyImport_FrozenModules = frozenModules;
	
	res = Py_FrozenMain(); // res = Py_Main(argc, argv_copy);
	
	return res;
}
// #endif
