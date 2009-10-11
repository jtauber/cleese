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

#undef PyObject_GC_Track
#undef PyObject_GC_UnTrack
#undef PyObject_GC_Del
#undef _PyObject_GC_Malloc

void
PyObject_GC_Track(void *op)
{
	_PyObject_GC_TRACK(op);
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
