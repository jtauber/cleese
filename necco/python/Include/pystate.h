#ifndef Py_PYSTATE_H
#define Py_PYSTATE_H

struct _ts; /* Forward */
struct _is; /* Forward */

typedef struct _is {

  struct _is *next;
  struct _ts *tstate_head;

  PyObject *modules;
  PyObject *sysdict; /* not used */
  PyObject *builtins;

  PyObject *codec_search_path; /* not used */
  PyObject *codec_search_cache; /* not used */
  PyObject *codec_error_registry; /* not used */

} PyInterpreterState;

/* State unique per thread */

struct _frame; /* Avoid including frameobject.h */

/* Py_tracefunc return -1 when raising an exception, or 0 for success. */
typedef int (*Py_tracefunc)(PyObject *, struct _frame *, int, PyObject *);

typedef struct _ts {
	
  struct _ts *next;
  PyInterpreterState *interp;

  struct _frame *frame;
  int recursion_depth;
  int tracing;
  int use_tracing;

  Py_tracefunc c_profilefunc;
  Py_tracefunc c_tracefunc;
  PyObject *c_profileobj;
  PyObject *c_traceobj;

  PyObject *curexc_type;
  PyObject *curexc_value;
  PyObject *curexc_traceback;

  PyObject *exc_type;
  PyObject *exc_value;
  PyObject *exc_traceback;

  PyObject *dict;

  int tick_counter;
  int gilstate_counter;

  PyObject *async_exc; /* Asynchronous exception to raise */
  long thread_id; /* Thread id where this tstate was created */

  /* XXX signal handlers should also be here */

} PyThreadState;

PyAPI_FUNC(PyInterpreterState *) PyInterpreterState_New(void);
PyAPI_FUNC(void) PyInterpreterState_Clear(PyInterpreterState *);
PyAPI_FUNC(void) PyInterpreterState_Delete(PyInterpreterState *);

PyAPI_FUNC(PyThreadState *) PyThreadState_New(PyInterpreterState *);
PyAPI_FUNC(void) PyThreadState_Clear(PyThreadState *);
PyAPI_FUNC(void) PyThreadState_Delete(PyThreadState *);

PyAPI_FUNC(PyThreadState *) PyThreadState_Get(void);
PyAPI_FUNC(PyThreadState *) PyThreadState_Swap(PyThreadState *);
PyAPI_FUNC(PyObject *) PyThreadState_GetDict(void);

PyAPI_DATA(PyThreadState *) _PyThreadState_Current;

#define PyThreadState_GET() (_PyThreadState_Current)

typedef struct _frame *(*PyThreadFrameGetter)(PyThreadState *self_);

/* hook for PyEval_GetFrame(), requested for Psyco */
PyAPI_DATA(PyThreadFrameGetter) _PyThreadState_GetFrame;

#endif /* !Py_PYSTATE_H */
