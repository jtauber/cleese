#include "Python.h"

#include "compile.h"
#include "frameobject.h"
#include "opcode.h"

static PyFrameObject *free_list = NULL;

PyTypeObject PyFrame_Type = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,
    "frame",
    sizeof(PyFrameObject),
    sizeof(PyObject *),
    0, //(destructor)frame_dealloc,         /* tp_dealloc */
    0,                  /* tp_print */
    0,                  /* tp_getattr */
    0,                  /* tp_setattr */
    0,                  /* tp_compare */
    0,                  /* tp_repr */
    0,                  /* tp_as_number */
    0,                  /* tp_as_sequence */
    0,                  /* tp_as_mapping */
    0,                  /* tp_hash */
    0,                  /* tp_call */
    0,                  /* tp_str */
    PyObject_GenericGetAttr,        /* tp_getattro */
    0, //PyObject_GenericSetAttr,       /* tp_setattro */
    0,                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,/* tp_flags */
    0,                          /* tp_doc */
    0, //(traverseproc)frame_traverse,      /* tp_traverse */
    0, //(inquiry)frame_clear,          /* tp_clear */
    0,                  /* tp_richcompare */
    0,                  /* tp_weaklistoffset */
    0,                  /* tp_iter */
    0,                  /* tp_iternext */
    0,                  /* tp_methods */
    0, //frame_memberlist,          /* tp_members */
    0, //frame_getsetlist,          /* tp_getset */
    0,                  /* tp_base */
    0,                  /* tp_dict */
};

static PyObject *builtin_object;

int _PyFrame_Init()
{
    void *builtin_object = PyString_InternFromString("__builtins__");
    return (builtin_object != NULL);
}

PyFrameObject *
PyFrame_New(PyThreadState *tstate, PyCodeObject *code, PyObject *globals, 
        PyObject *locals)
{
    LOG("> PyFrame_New\n"); {
    PyFrameObject *back = tstate->frame;
    PyFrameObject *f;
    PyObject *builtins;
    int extras, ncells, nfrees;
    ncells = PyTuple_GET_SIZE(code->co_cellvars);
    nfrees = PyTuple_GET_SIZE(code->co_freevars);
    extras = code->co_stacksize + code->co_nlocals + ncells + nfrees;
    
    if (back == NULL || back->f_globals != globals) {
        
        /* JKT: how did builtin_object get to be NULL again!?!? */
        if (builtin_object == NULL)
            builtin_object = PyString_InternFromString("__builtins__");
        
        builtins = PyDict_GetItem(globals, builtin_object);
        if (builtins) {
            if (PyModule_Check(builtins)) {
                builtins = PyModule_GetDict(builtins);
            }
            else if (!PyDict_Check(builtins))
                builtins = NULL;
        }
        if (builtins == NULL) {
            /* No builtins!  Make up a minimal one 
               Give them 'None', at least. */
            builtins = PyDict_New();
            if (builtins == NULL || 
                PyDict_SetItemString(
                    builtins, "None", Py_None) < 0)
                return NULL;
        }
        else
            Py_INCREF(builtins);
    
    }
    else {
        /* If we share the globals, we share the builtins.
           Save a lookup and a call. */
        builtins = back->f_builtins;
        Py_INCREF(builtins);
    }
    
    if (free_list == NULL) {
        f = PyObject_GC_NewVar(PyFrameObject, &PyFrame_Type, extras);
        if (f == NULL)
            return NULL;
    }
    else {
        /* NOT IMPLEMENTED */
    }
    
    f->f_builtins = builtins;
    Py_XINCREF(back);
    f->f_back = back;
    Py_INCREF(code);
    f->f_code = code;
    Py_INCREF(globals);
    f->f_globals = globals;
    
    /* Most functions have CO_NEWLOCALS and CO_OPTIMIZED set. */
    if ((code->co_flags & (CO_NEWLOCALS | CO_OPTIMIZED)) == 
        (CO_NEWLOCALS | CO_OPTIMIZED))
        locals = NULL; /* PyFrame_Fast2Locals() will set. */
    else if (code->co_flags & CO_NEWLOCALS) {
        locals = PyDict_New();
        if (locals == NULL) {
            Py_DECREF(f);
            return NULL;
        }
    }
    else {
        if (locals == NULL)
            locals = globals;
        Py_INCREF(locals);
    }
    
    f->f_locals = locals;
    f->f_trace = NULL;
    f->f_exc_type = f->f_exc_value = f->f_exc_traceback = NULL;
    f->f_tstate = tstate;
    
    f->f_lasti = -1;
    f->f_lineno = code->co_firstlineno;
    f->f_restricted = (builtins != tstate->interp->builtins);
    f->f_iblock = 0;
    f->f_nlocals = code->co_nlocals;
    f->f_stacksize = code->co_stacksize;
    f->f_ncells = ncells;
    f->f_nfreevars = nfrees;
    
    extras = f->f_nlocals + ncells + nfrees;
    memset(f->f_localsplus, 0, extras * sizeof(f->f_localsplus[0]));
    
    f->f_valuestack = f->f_localsplus + extras;
    f->f_stacktop = f->f_valuestack;
    
    _PyObject_GC_TRACK(f);
    
    LOG("< PyFrame_New\n");
    return f;
}}

void
PyFrame_Fini(void)
{
    /* TO DO */
}

/* Block management */

void
PyFrame_BlockSetup(PyFrameObject *f, int type, int handler, int level)
{
    PyTryBlock *b;
    if (f->f_iblock >= CO_MAXBLOCKS)
        Py_FatalError("XXX block stack overflow");
    b = &f->f_blockstack[f->f_iblock++];
    b->b_type = type;
    b->b_level = level;
    b->b_handler = handler;
}

PyTryBlock *
PyFrame_BlockPop(PyFrameObject *f)
{
    PyTryBlock *b;
    if (f->f_iblock <= 0)
        Py_FatalError("XXX block stack underflow");
    b = &f->f_blockstack[--f->f_iblock];
    return b;
}
