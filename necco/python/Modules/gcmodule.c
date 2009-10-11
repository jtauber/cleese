#include "Python.h"

#define AS_GC(o) ((PyGC_Head *)(o)-1)

#define FROM_GC(g) ((PyObject *)(((PyGC_Head *)g)+1))

struct gc_generation {
	PyGC_Head head;
	int threshold;
	int count;
};

#define NUM_GENERATIONS 3
#define GEN_HEAD(n) (&generations[n].head)

static struct gc_generation generations[NUM_GENERATIONS] = {
	{{{GEN_HEAD(0), GEN_HEAD(0), 0}}, 700, 0},
	{{{GEN_HEAD(1), GEN_HEAD(1), 0}}, 10,  0},
	{{{GEN_HEAD(2), GEN_HEAD(2), 0}}, 10,  0},
};

PyGC_Head *_PyGC_generation0 = GEN_HEAD(0);

#define GC_UNTRACKED _PyGC_REFS_UNTRACKED
#define GC_REACHABLE			_PyGC_REFS_REACHABLE
#define GC_TENTATIVELY_UNREACHABLE	_PyGC_REFS_TENTATIVELY_UNREACHABLE

#define IS_TRACKED(o) ((AS_GC(o))->gc.gc_refs != GC_UNTRACKED)
#define IS_REACHABLE(o) ((AS_GC(o))->gc.gc_refs == GC_REACHABLE)
#define IS_TENTATIVELY_UNREACHABLE(o) ( \
	(AS_GC(o))->gc.gc_refs == GC_TENTATIVELY_UNREACHABLE)


static void
gc_list_remove(PyGC_Head *node)
{
	node->gc.gc_prev->gc.gc_next = node->gc.gc_next;
	node->gc.gc_next->gc.gc_prev = node->gc.gc_prev;
	node->gc.gc_next = NULL; /* object is not currently tracked */
}


#undef PyObject_GC_Track
#undef PyObject_GC_UnTrack
#undef PyObject_GC_Del
#undef _PyObject_GC_Malloc

void
PyObject_GC_Track(void *op)
{
	_PyObject_GC_TRACK(op);
}

void
PyObject_GC_UnTrack(void *op)
{
	/* Obscure:  the Py_TRASHCAN mechanism requires that we be able to
	 * call PyObject_GC_UnTrack twice on an object.
	 */
	if (IS_TRACKED(op))
		_PyObject_GC_UNTRACK(op);
}

/* for binary compatibility with 2.2 */
void
_PyObject_GC_UnTrack(PyObject *op)
{
    PyObject_GC_UnTrack(op);
}

PyObject *
_PyObject_GC_Malloc(size_t basicsize)
{
	LOG("> _PyObject_GC_Malloc\n"); {
	PyObject *op;
	PyGC_Head *g = PyObject_MALLOC(sizeof(PyGC_Head) + basicsize);

	if (g == NULL)
	  return NULL; /* NO MEM */
	g->gc.gc_refs = GC_UNTRACKED;
	generations[0].count++; /* number of allocated GC objects */

	/* actual collection elides */


	op = FROM_GC(g);
	LOG("< _PyObject_GC_Malloc\n");
	return op;
}}

PyObject *
_PyObject_GC_New(PyTypeObject *tp)
{
	LOG("> _PyObject_GC_New\n"); {
	PyObject *op = _PyObject_GC_Malloc(_PyObject_SIZE(tp));
	if (op != NULL)
		op = PyObject_INIT(op, tp);
	LOG("< _PyObject_GC_New\n");
	return op;
}}

PyVarObject *
_PyObject_GC_NewVar(PyTypeObject *tp, int nitems)
{
	const size_t size = _PyObject_VAR_SIZE(tp, nitems);
	PyVarObject *op = (PyVarObject *) _PyObject_GC_Malloc(size);
	if (op != NULL)
		op = PyObject_INIT_VAR(op, tp, nitems);
	return op;
}

PyVarObject *
_PyObject_GC_Resize(PyVarObject *op, int nitems)
{
	const size_t basicsize = _PyObject_VAR_SIZE(op->ob_type, nitems);
	PyGC_Head *g = AS_GC(op);
	g = PyObject_REALLOC(g,  sizeof(PyGC_Head) + basicsize);
	if (g == NULL)
		return (PyVarObject *)PyErr_NoMemory();
	op = (PyVarObject *) FROM_GC(g);
	op->ob_size = nitems;
	return op;
}

void
PyObject_GC_Del(void *op)
{
	PyGC_Head *g = AS_GC(op);
	if (IS_TRACKED(op))
		gc_list_remove(g);
	if (generations[0].count > 0) {
		generations[0].count--;
	}
	PyObject_FREE(g);
}

/* for binary compatibility with 2.2 */
#undef _PyObject_GC_Del
void
_PyObject_GC_Del(PyObject *op)
{
    PyObject_GC_Del(op);
}
