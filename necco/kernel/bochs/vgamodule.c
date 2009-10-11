#include "Python.h"

static unsigned char font0[8192];
static unsigned char savebuffer[80*25*2];
extern unsigned char bootscreen[];

PyObject *
_VGA_Init(void)
{
	PyObject *mod, *dict;
	mod = Py_InitModule4("vga", NULL,
			     NULL/*doc*/, (PyObject *)NULL,
			     PYTHON_API_VERSION);

	dict = PyModule_GetDict(mod);

	#define SETBUILTIN(NAME, OBJECT) \
	if (PyDict_SetItemString(dict, NAME, (PyObject *)OBJECT) < 0)	\
		return NULL;

	SETBUILTIN("framebuffer",
		PyBuffer_FromReadWriteMemory((void *)0xa0000, 0x10000));
	SETBUILTIN("textbuffer",
		PyBuffer_FromReadWriteMemory((void *)0xb8000, 80*25*2));
	SETBUILTIN("savebuffer",
		PyBuffer_FromReadWriteMemory((void *)savebuffer, 80*25*2));
	SETBUILTIN("splashscreen",
		PyBuffer_FromMemory(bootscreen, 0x10000));
	SETBUILTIN("font0",
		PyBuffer_FromReadWriteMemory((void *)font0, 8192));

	init_screen();

	return mod;
}
