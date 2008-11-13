/* 

derived from Modules/python.c but doesn't take command-line arguments
and calls FrozenMain after setting up own frozen module(s)

*/

#include "Python.h"

unsigned char x[] = {
        99,0,0,0,0,0,0,0,0,2,0,0,0,64,0,0,
        0,115,13,0,0,0,100,0,0,100,0,0,23,71,72,100,
        1,0,83,40,2,0,0,0,105,1,0,0,0,78,40,0,
        0,0,0,40,0,0,0,0,40,0,0,0,0,40,0,0,
        0,0,115,7,0,0,0,116,101,115,116,46,112,121,115,1,
        0,0,0,63,1,0,0,0,115,0,0,0,0,
};

static struct _frozen frozenModules[] = {
        {"__main__", x, 93}, 
	{0, 0, 0} /* sentinel */
};

struct _frozen *PyImport_FrozenModules = frozenModules;

int
main(void)
{
	int res;
	
	res = Py_FrozenMain();
	
	return res;
}
