#include "Python.h"

#define VLEN    8

static PyObject *d = NULL;
static PyObject *t = NULL;
static PyObject *vec[VLEN];

/* - - - - - - - - - - - - - - - - - - - - - - - - */
int isr_mask;       /* these are here to mask  */
void *ticker;       /* a link problem w/ DJGPP */
/* - - - - - - - - - - - - - - - - - - - - - - - - */

extern int isrs_pending();
extern int initirqs();

int python_isr(void *arg)
{
    PyObject **p = vec;
    int pending = isrs_pending();
    
    if(pending) {
        int n;
        for(n = 0; n < VLEN; ++n)   {
            if(*p && (pending & 1)) {
#if 0
                /* hmmm, doesn't quite work yet */
                Py_DECREF(PyObject_Call(*p, NULL, NULL));
#else
                extern PyObject *PyEval_EvalCode();
                Py_DECREF(PyEval_EvalCode(
                        PyFunction_GET_CODE(*p), d, d));
#endif
            }
            pending >>= 1;
            p++;
        }
    }
    return 0;
}

static PyObject *
isr_setvec(PyObject *self, PyObject *args)
{
    int n;
    
    if(!d)  {
        PyObject *s = PyThreadState_Get()->interp->modules;
        PyObject *m = PyDict_GetItemString(s, "__main__");
        
        d = PyModule_GetDict(m);
    }
    
    for(n = 0; n < VLEN; ++n)
        vec[n] = NULL;
    
    if(!PyArg_UnpackTuple(args, "setvec", 0, VLEN,
            vec+0, vec+1, vec+2, vec+3,
            vec+4, vec+5, vec+6, vec+7))
        return NULL;
    
    initirqs();
    
    Py_INCREF(Py_True);
    return Py_True;
}

static PyMethodDef isr_methods[] = {
    {"setvec",  isr_setvec,     METH_VARARGS, NULL/*doc*/},
    {NULL,      NULL},
};

PyObject *
_ISR_Init(void)
{
    PyObject *mod, *dict;
    
    mod = Py_InitModule4("isr", isr_methods,
                 NULL/*doc*/, (PyObject *)NULL,
                 PYTHON_API_VERSION);
    
    dict = PyModule_GetDict(mod);
    
    #define SETBUILTIN(NAME, OBJECT) \
    if (PyDict_SetItemString(dict, NAME, (PyObject *)OBJECT) < 0)   \
        return NULL;
    
    t = PyInt_FromLong(0);
    ticker = &(PyInt_AS_LONG(t));
    SETBUILTIN("ticker", t);
    
    return mod;
}
