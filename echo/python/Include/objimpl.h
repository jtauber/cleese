#ifndef Py_OBJIMPL_H
#define Py_OBJIMPL_H

#include "pymem.h"

#define PyObject_MALLOC  PyMem_MALLOC

PyAPI_FUNC(PyObject *) PyObject_Init(PyObject *, PyTypeObject *);

#define PyObject_INIT(op, typeobj) \
	( (op)->ob_type = (typeobj), _Py_NewReference((PyObject *)(op)), (op) )

#define PyObject_INIT_VAR(op, typeobj, size) \
	( (op)->ob_size = (size), PyObject_INIT((op), (typeobj)) )

#define _PyObject_SIZE(typeobj) ( (typeobj)->tp_basicsize )

#define _PyObject_VAR_SIZE(typeobj, nitems)	\
	(size_t)				\
	( ( (typeobj)->tp_basicsize +		\
	    (nitems)*(typeobj)->tp_itemsize +	\
	    (3)			\
	  ) & ~(3)		\
	)

#define PyObject_NEW(type, typeobj) \
( (type *) PyObject_Init( \
	(PyObject *) PyObject_MALLOC( _PyObject_SIZE(typeobj) ), (typeobj)) )


PyAPI_FUNC(PyVarObject *) _PyObject_GC_Resize(PyVarObject *, int);
#define PyObject_GC_Resize(type, op, n) \
		( (type *) _PyObject_GC_Resize((PyVarObject *)(op), (n)) )

/* GC information is stored BEFORE the object structure. */
typedef union _gc_head {
        struct {
                union _gc_head *gc_next;
                union _gc_head *gc_prev;
                int gc_refs;
        } gc;
        long double dummy;  /* force worst-case alignment */
} PyGC_Head;

extern PyGC_Head *_PyGC_generation0;

#define _Py_AS_GC(o) ((PyGC_Head *)(o)-1)

#define _PyGC_REFS_UNTRACKED                    (-2)
#define _PyGC_REFS_REACHABLE                    (-3)
#define _PyGC_REFS_TENTATIVELY_UNREACHABLE      (-4)

/* Tell the GC to track this object.  NB: While the object is tracked the
 * collector it must be safe to call the ob_traverse method. */
/*
#define _PyObject_GC_TRACK(o) do { \
        PyGC_Head *g = _Py_AS_GC(o); \
        if (g->gc.gc_refs != _PyGC_REFS_UNTRACKED) \
                Py_FatalError("GC object already tracked"); \
        g->gc.gc_refs = _PyGC_REFS_REACHABLE; \
        g->gc.gc_next = _PyGC_generation0; \
        g->gc.gc_prev = _PyGC_generation0->gc.gc_prev; \
        g->gc.gc_prev->gc.gc_next = g; \
        _PyGC_generation0->gc.gc_prev = g; \
    } while (0);
*/
#define _PyObject_GC_TRACK(o)

PyAPI_FUNC(PyObject *) _PyObject_GC_Malloc(size_t);
PyAPI_FUNC(PyObject *) _PyObject_GC_New(PyTypeObject *);
PyAPI_FUNC(PyVarObject *) _PyObject_GC_NewVar(PyTypeObject *, int);
PyAPI_FUNC(void) PyObject_GC_Track(void *);

#define PyObject_GC_New(type, typeobj) \
	( (type *) _PyObject_GC_New(typeobj) )
#define PyObject_GC_NewVar(type, typeobj, n) \
        ( (type *) _PyObject_GC_NewVar((typeobj), (n)) )

#endif /* !Py_OBJIMPL_H */
