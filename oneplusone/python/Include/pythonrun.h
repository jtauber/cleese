/* Interfaces to parse and execute pieces of python code */

#ifndef Py_PYTHONRUN_H
#define Py_PYTHONRUN_H

PyAPI_FUNC(void) Py_Initialize(void);
PyAPI_FUNC(void) Py_Finalize(void);

/* Bootstrap */
PyAPI_FUNC(void) Py_Main(void);

/* Internal -- various one-time initialization */
PyAPI_FUNC(int) _PyFrame_Init(void);
PyAPI_FUNC(int) _PyInt_Init(void);

/* Various internal finalizers */
PyAPI_FUNC(void) PyFrame_Fini(void);
PyAPI_FUNC(void) PyInt_Fini(void);

#endif /* !Py_PYTHONRUN_H */
