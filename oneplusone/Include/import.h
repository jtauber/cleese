#ifndef Py_IMPORT_H
#define Py_IMPORT_H

PyAPI_FUNC(PyObject *) PyImport_ExecCodeModule(char *name, PyObject *co);
PyAPI_FUNC(PyObject *) PyImport_GetModuleDict(void);
PyAPI_FUNC(PyObject *) PyImport_AddModule(char *name);
PyAPI_FUNC(int)        PyImport_ImportFrozenModule(char *);

struct _frozen {
  char *name;
  unsigned char *code;
  int size;
};

struct _frozen * PyImport_FrozenModules;

#endif /* !Py_IMPORT_H */
