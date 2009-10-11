#include "Python.h"

void
main(void)
{
        clrscr();

	/* This file cannot have string constants because they get
           placed before the code and so will attempt to be executed */
        char banner[] = {'C', 'l', 'e', 'e', 's', 'e', ':', '\0'};
        print(banner);

        Py_Main();

        for(;;);
}
