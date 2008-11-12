/* 

derived from Modules/python.c but doesn't take command-line arguments
and calls FrozenMain after setting up own frozen module(s)

*/

#include "Python.h"

static struct _frozen frozenModules[] = {
//        {"__main__", <bytes>, <len>}, 
//        {0, 0, 0} /* sentinel */
};

struct _frozen *PyImport_FrozenModules = frozenModules;

int
main(void)
{
	int res;
	
	res = Py_FrozenMain();
	
	return res;
}
