#ifndef Py_OBJIMPL_H
#define Py_OBJIMPL_H

#include "pymem.h"

PyAPI_FUNC(void *) PyObject_Malloc(size_t);
PyAPI_FUNC(void *) PyObject_Realloc(void *, size_t);

#define PyObject_Free free /* for now */
//PyAPI_FUNC(void) PyObject_Free(void *);

#define PyObject_MALLOC  PyMem_MALLOC
#define PyObject_REALLOC	PyMem_REALLOC
/* This is an odd one!  For backward compatability with old extensions, the
   PyMem "release memory" functions have to invoke the object allocator's
   free() function.  When pymalloc isn't enabled, that leaves us using
   the platform free(). */
#define PyObject_FREE		free

#define PyObject_Del PyObject_FREE /* for now */
#define PyObject_DEL PyObject_FREE

#define _PyObject_Del		PyObject_Free

PyAPI_FUNC(PyObject *) PyObject_Init(PyObject *, PyTypeObject *);
PyAPI_FUNC(PyVarObject *) PyObject_InitVar(PyVarObject *,
                                                 PyTypeObject *, int);
PyAPI_FUNC(PyObject *) _PyObject_New(PyTypeObject *);
PyAPI_FUNC(PyVarObject *) _PyObject_NewVar(PyTypeObject *, int);

#define PyObject_New(type, typeobj) \
		( (type *) _PyObject_New(typeobj) )
#define PyObject_NewVar(type, typeobj, n) \
		( (type *) _PyObject_NewVar((typeobj), (n)) )


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


#define PyObject_NEW_VAR(type, typeobj, n) \
( (type *) PyObject_InitVar( \
      (PyVarObject *) PyObject_MALLOC(_PyObject_VAR_SIZE((typeobj),(n)) ),\
      (typeobj), (n)) )

/*
 * Garbage Collection Support
 * ==========================
 */

/* C equivalent of gc.collect(). */
long PyGC_Collect(void);

/* Test if a type has a GC head */
#define PyType_IS_GC(t) PyType_HasFeature((t), Py_TPFLAGS_HAVE_GC)

/* Test if an object has a GC head */
#define PyObject_IS_GC(o) (PyType_IS_GC((o)->ob_type) && \
	((o)->ob_type->tp_is_gc == NULL || (o)->ob_type->tp_is_gc(o)))

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
#define _PyObject_GC_TRACK(o) /* Nothing */
#define _PyObject_GC_UNTRACK(o) /* Nothing */


PyAPI_FUNC(PyObject *) _PyObject_GC_Malloc(size_t);
PyAPI_FUNC(PyObject *) _PyObject_GC_New(PyTypeObject *);
PyAPI_FUNC(PyVarObject *) _PyObject_GC_NewVar(PyTypeObject *, int);
PyAPI_FUNC(void) PyObject_GC_Track(void *);
PyAPI_FUNC(void) PyObject_GC_UnTrack(void *);
PyAPI_FUNC(void) PyObject_GC_Del(void *);

#define PyObject_GC_New(type, typeobj) \
	( (type *) _PyObject_GC_New(typeobj) )
#define PyObject_GC_NewVar(type, typeobj, n) \
        ( (type *) _PyObject_GC_NewVar((typeobj), (n)) )

/* Test if a type supports weak references */
#define PyType_SUPPORTS_WEAKREFS(t) \
        (PyType_HasFeature((t), Py_TPFLAGS_HAVE_WEAKREFS) \
         && ((t)->tp_weaklistoffset > 0))

#define PyObject_GET_WEAKREFS_LISTPTR(o) \
	((PyObject **) (((char *) (o)) + (o)->ob_type->tp_weaklistoffset))

#endif /* !Py_OBJIMPL_H */
