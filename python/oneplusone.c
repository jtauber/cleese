/* 

derived from Modules/python.c but doesn't take command-line arguments
and calls FrozenMain after setting up own frozen module(s)

*/

#include "Python.h"

unsigned char x[] = {
	99,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,
	0,64,0,0,0,115,14,0,0,0,101,0,0,100,2,0,
	131,1,0,1,100,1,0,83,40,3,0,0,0,105,1,0,
	0,0,78,105,2,0,0,0,40,1,0,0,0,117,5,0,
	0,0,112,114,105,110,116,40,0,0,0,0,40,0,0,0,
	0,40,0,0,0,0,117,22,0,0,0,60,102,114,111,122,
	101,110,32,111,110,101,112,108,117,115,111,110,101,46,112,121,
	62,117,8,0,0,0,60,109,111,100,117,108,101,62,1,0,
	0,0,115,0,0,0,0,
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
