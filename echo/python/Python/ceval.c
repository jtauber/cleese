#include "Python.h"

#include "compile.h"
#include "frameobject.h"
#include "eval.h"
#include "opcode.h"

/* Forward declarations */
static PyObject *eval_frame(PyFrameObject *);
static PyObject *call_function(PyObject ***, int);
static PyObject *fast_function(PyObject *, PyObject ***, int, int, int);
static PyObject *do_call(PyObject *, PyObject ***, int, int);
static PyObject *update_keyword_args(PyObject *, int, PyObject ***,PyObject *);
static PyObject *load_args(PyObject ***, int);

/* @@@ need to investigate need for this */
PyObject *PyEval_CallObjectWithKeywords(PyObject *, PyObject *, PyObject *);
static int assign_slice(PyObject *, PyObject *, PyObject *, PyObject *);

/* Mechanism whereby asynchronously executing callbacks (e.g. UNIX
   signal handlers or Mac I/O completion routines) can schedule calls
   to a function to be called synchronously.
   The synchronous function is called with one void* argument.
   It should return 0 for success or -1 for failure -- failure should
   be accompanied by an exception.

   If registry succeeds, the registry function returns 0; if it fails
   (e.g. due to too many pending calls) it returns -1 (without setting
   an exception condition).

   Note that because registry may occur from within signal handlers,
   or other asynchronous events, calling malloc() is unsafe!

#ifdef WITH_THREAD
   Any thread can schedule pending calls, but only the main thread
   will execute them.
#endif

   XXX WARNING!  ASYNCHRONOUSLY EXECUTING CODE!
   There are two possible race conditions:
   (1) nested asynchronous registry calls;
   (2) registry calls made while pending calls are being processed.
   While (1) is very unlikely, (2) is a real possibility.
   The current code is safe against (2), but not against (1).
   The safety against (2) is derived from the fact that only one
   thread (the main thread) ever takes things out of the queue.

   XXX Darn!  With the advent of thread state, we should have an array
   of pending calls per thread in the thread state!  Later...
*/

#define NPENDINGCALLS 32
static struct {
	int (*func)(void *);
	void *arg;
} pendingcalls[NPENDINGCALLS];
static volatile int pendingfirst = 0;
static volatile int pendinglast = 0;
static volatile int things_to_do = 0;

int
Py_AddPendingCall(int (*func)(void *), void *arg)
{
	static int busy = 0;
	int i, j;
	/* XXX Begin critical section */
	/* XXX If you want this to be safe against nested
	   XXX asynchronous calls, you'll have to work harder! */
	if (busy)
		return -1;
	busy = 1;
	i = pendinglast;
	j = (i + 1) % NPENDINGCALLS;
	if (j == pendingfirst) {
		busy = 0;
		return -1; /* Queue full */
	}
	pendingcalls[i].func = func;
	pendingcalls[i].arg = arg;
	pendinglast = j;

	_Py_Ticker = 0;
	things_to_do = 1; /* Signal main loop */
	busy = 0;
	/* XXX End critical section */
	return 0;
}

int
Py_MakePendingCalls(void)
{
	static int busy = 0;
	if (busy)
		return 0;
	busy = 1;
	things_to_do = 0;
	for (;;) {
		int i;
		int (*func)(void *);
		void *arg;
		i = pendingfirst;
		if (i == pendinglast)
			break; /* Queue empty */
		func = pendingcalls[i].func;
		arg = pendingcalls[i].arg;
		pendingfirst = (i + 1) % NPENDINGCALLS;
		if (func(arg) < 0) {
			busy = 0;
			things_to_do = 1; /* We're not done yet */
			return -1;
		}
	}
	busy = 0;
	return 0;
}

/* The interpreter's recursion limit */

static int recursion_limit = 1000;

/* Status code for main loop (reason for stack unwind) */

enum why_code {
		WHY_NOT,	/* No error */
		WHY_EXCEPTION,	/* Exception occurred */
		WHY_RERAISE,	/* Exception re-raised by 'finally' */
		WHY_RETURN,	/* 'return' statement */
		WHY_BREAK,	/* 'break' statement */
		WHY_CONTINUE,	/* 'continue' statement */
		WHY_YIELD	/* 'yield' operator */
};

/* for manipulating the thread switch and periodic "stuff" - used to be
   per thread, now just a pair o' globals */
int _Py_CheckInterval = 100;
volatile int _Py_Ticker = 100;

PyObject *
PyEval_EvalCode(PyCodeObject *co, PyObject *globals, PyObject *locals)
{
	/* XXX raise SystemError if globals is NULL */
	return PyEval_EvalCodeEx(co,
			  globals, locals,
			  (PyObject **)NULL, 0,
			  (PyObject **)NULL, 0,
			  (PyObject **)NULL, 0,
			  NULL);
}

static PyObject *
eval_frame(PyFrameObject *f)
{
	LOG("> eval_frame\n"); {
	PyObject **stack_pointer; /* Next free slot in value stack */
	register unsigned char *next_instr;
	register int opcode=0;	/* Current opcode */
	register int oparg=0;	/* Current opcode argument, if any */
	register enum why_code why; /* Reason for block stack unwind */
	register int err;	/* Error status -- nonzero if error */
	register PyObject *x;	/* Result object -- NULL if error */
	register PyObject *t, *u, *v;	/* Temporary objects popped off stack */
	register PyObject *w;
	register PyObject **fastlocals, **freevars;
	PyObject *retval = NULL;	/* Return value */
	PyThreadState *tstate = PyThreadState_GET();
	PyCodeObject *co;

	unsigned char *first_instr;
	PyObject *names;
	PyObject *consts;

/* Tuple access macros */

#define GETITEM(v, i) PyTuple_GET_ITEM((PyTupleObject *)(v), (i))

/* Code access macros */

#define INSTR_OFFSET()	(next_instr - first_instr)
#define NEXTOP()	(*next_instr++)
#define NEXTARG()	(next_instr += 2, (next_instr[-1]<<8) + next_instr[-2])
#define JUMPTO(x)	(next_instr = first_instr + (x))
#define JUMPBY(x)	(next_instr += (x))

/* OpCode prediction macros
	Some opcodes tend to come in pairs thus making it possible to predict
	the second code when the first is run.  For example, COMPARE_OP is often
	followed by JUMP_IF_FALSE or JUMP_IF_TRUE.  And, those opcodes are often
	followed by a POP_TOP.

	Verifying the prediction costs a single high-speed test of register
	variable against a constant.  If the pairing was good, then the
	processor has a high likelihood of making its own successful branch
	prediction which results in a nearly zero overhead transition to the
	next opcode.

	A successful prediction saves a trip through the eval-loop including
	its two unpredictable branches, the HASARG test and the switch-case.
*/

#define PREDICT(op)		if (*next_instr == op) goto PRED_##op
#define PREDICTED(op)		PRED_##op: next_instr++
#define PREDICTED_WITH_ARG(op)	PRED_##op: oparg = (next_instr[2]<<8) + \
				next_instr[1]; next_instr += 3

/* Stack manipulation macros */

#define STACK_LEVEL()	(stack_pointer - f->f_valuestack)
#define EMPTY()		(STACK_LEVEL() == 0)
#define TOP()		(stack_pointer[-1])
#define SECOND()	(stack_pointer[-2])
#define THIRD() 	(stack_pointer[-3])
#define FOURTH()	(stack_pointer[-4])
#define SET_TOP(v)	(stack_pointer[-1] = (v))
#define SET_SECOND(v)	(stack_pointer[-2] = (v))
#define SET_THIRD(v)	(stack_pointer[-3] = (v))
#define SET_FOURTH(v)	(stack_pointer[-4] = (v))
#define BASIC_STACKADJ(n)	(stack_pointer += n)
#define BASIC_PUSH(v)	(*stack_pointer++ = (v))
#define BASIC_POP()	(*--stack_pointer)

#define PUSH(v)		BASIC_PUSH(v)
#define POP()		BASIC_POP()
#define STACKADJ(n)	BASIC_STACKADJ(n)

/* Local variable macros */

#define GETLOCAL(i)	(fastlocals[i])

/* The SETLOCAL() macro must not DECREF the local variable in-place and
   then store the new value; it must copy the old value to a temporary
   value, then store the new value, and then DECREF the temporary value.
   This is because it is possible that during the DECREF the frame is
   accessed by other code (e.g. a __del__ method or gc.collect()) and the
   variable would be pointing to already-freed memory. */
#define SETLOCAL(i, value)	do { PyObject *tmp = GETLOCAL(i); \
				     GETLOCAL(i) = value; \
                                     Py_XDECREF(tmp); } while (0)

/* Start of code */

	if (f == NULL)
		return NULL;

	/* push frame */
	if (++tstate->recursion_depth > recursion_limit) {
		--tstate->recursion_depth;
		/* ERROR */
		tstate->frame = f->f_back;
		return NULL;
	}

	tstate->frame = f;

	/* tracing elided */

	co = f->f_code;
	names = co->co_names;
	consts = co->co_consts;
	fastlocals = f->f_localsplus;
	freevars = f->f_localsplus + f->f_nlocals;

	_PyCode_GETCODEPTR(co, &first_instr);

	/* An explanation is in order for the next line.

	   f->f_lasti now refers to the index of the last instruction
	   executed.  You might think this was obvious from the name, but
	   this wasn't always true before 2.3!  PyFrame_New now sets
	   f->f_lasti to -1 (i.e. the index *before* the first instruction)
	   and YIELD_VALUE doesn't fiddle with f_lasti any more.  So this
	   does work.  Promise. */
	next_instr = first_instr + f->f_lasti + 1;
	stack_pointer = f->f_stacktop;

	f->f_stacktop = NULL;	/* remains NULL unless yield suspends frame */

	why = WHY_NOT;
	err = 0;
	x = Py_None;	/* Not a reference, just anything non-NULL */
	w = NULL;

	for (;;) {

		/* Do periodic things.  Doing this every time through
		   the loop would add too much overhead, so we do it
		   only every Nth instruction.  We also do it if
		   ``things_to_do'' is set, i.e. when an asynchronous
		   event needs attention (e.g. a signal handler or
		   async I/O handler); see Py_AddPendingCall() and
		   Py_MakePendingCalls() above. */

		if (--_Py_Ticker < 0) {
			/* @@@ check for SETUP_FINALLY elided */

			_Py_Ticker = _Py_CheckInterval;
			tstate->tick_counter++;
			if (things_to_do) {
				if (Py_MakePendingCalls() < 0) {
					why = WHY_EXCEPTION;
					goto on_error;
				}
			}
		}

	fast_next_opcode:
		f->f_lasti = INSTR_OFFSET();

		/* Extract opcode and argument */

		opcode = NEXTOP();
		if (HAS_ARG(opcode))
			oparg = NEXTARG();

		/* Main switch on opcode */

		switch (opcode) {

		/* BEWARE!
		   It is essential that any operation that fails sets either
		   x to NULL, err to nonzero, or why to anything but WHY_NOT,
		   and that no operation that succeeds does this! */

		/* case STOP_CODE: this is an error! */

		case LOAD_FAST:
			x = GETLOCAL(oparg);
			if (x != NULL) {
				Py_INCREF(x);
				PUSH(x);
				goto fast_next_opcode;
			}
			/* ERROR? */
			break;

                case STORE_FAST:
                        v = POP();
                        SETLOCAL(oparg, v);
                        continue;

		case LOAD_CONST:
			x = GETITEM(consts, oparg);
			Py_INCREF(x);
			PUSH(x);
			goto fast_next_opcode;

		PREDICTED(POP_TOP);
		case POP_TOP:
			v = POP();
			Py_DECREF(v);
			goto fast_next_opcode;

		case UNARY_NOT:
			v = TOP();
			err = PyObject_IsTrue(v);
			Py_DECREF(v);
			if (err == 0) {
				Py_INCREF(Py_True);
				SET_TOP(Py_True);
				continue;
			}
			else if (err > 0) {
				Py_INCREF(Py_False);
				SET_TOP(Py_False);
				err = 0;
				continue;
			}
			STACKADJ(-1);
			break;

		case BINARY_MODULO:
			w = POP();
			v = TOP();
			x = PyNumber_Remainder(v, w);
			Py_DECREF(v);
			Py_DECREF(w);
			SET_TOP(x);
			if (x != NULL) continue;
			break;
			
		case BINARY_ADD:
			w = POP();
			v = TOP();
			if (PyInt_CheckExact(v) && PyInt_CheckExact(w)) {
				/* INLINE: int + int */
				register long a, b, i;
				a = PyInt_AS_LONG(v);
				b = PyInt_AS_LONG(w);
				i = a + b;
				if ((i^a) < 0 && (i^b) < 0)
					goto slow_add;
				x = PyInt_FromLong(i);
			}
			else {
			  slow_add:
				Py_FatalError("slow add not supported.");
			}
			Py_DECREF(v);
			Py_DECREF(w);
			SET_TOP(x);
			if (x != NULL) continue;
			break;

                case STORE_SLICE+0:
                case STORE_SLICE+1:
                case STORE_SLICE+2:
                case STORE_SLICE+3:
                        if ((opcode-STORE_SLICE) & 2)
                                w = POP();
                        else
                                w = NULL;
                        if ((opcode-STORE_SLICE) & 1)
                                v = POP();
                        else
                                v = NULL;
                        u = POP();
                        t = POP();
                        err = assign_slice(u, v, w, t); /* u[v:w] = t */
                        Py_DECREF(t);
                        Py_DECREF(u);
                        Py_XDECREF(v);
                        Py_XDECREF(w);
                        if (err == 0) continue;
                        break;

                case STORE_SUBSCR:
                        w = POP();
                        v = POP();
                        u = POP();
                        /* v[w] = u */
                        err = PyObject_SetItem(v, w, u);
                        Py_DECREF(u);
                        Py_DECREF(v);
                        Py_DECREF(w);
                        if (err == 0) continue;
                        break;

                case BINARY_SUBSCR:
                        w = POP();
                        v = TOP();
                        if (PyList_CheckExact(v) && PyInt_CheckExact(w)) {
                                /* INLINE: list[int] */
                                long i = PyInt_AsLong(w);
                                if (i < 0)
                                        i += PyList_GET_SIZE(v);
                                if (i < 0 ||
                                    i >= PyList_GET_SIZE(v)) {
                                        /* ERROR */
                                        printf("list index out of range\n");
                                        x = NULL;
                                }
                                else {
                                        x = PyList_GET_ITEM(v, i);
                                        Py_INCREF(x);
                                }
                        }
                        else
                                x = PyObject_GetItem(v, w);
                        Py_DECREF(v);
                        Py_DECREF(w);
                        SET_TOP(x);
                        if (x != NULL) continue;
                        break;

		case BINARY_AND:
			w = POP();
			v = TOP();
			x = PyNumber_And(v, w);
			Py_DECREF(v);
			Py_DECREF(w);
			SET_TOP(x);
			if (x != NULL) continue;
			break;

		case PRINT_ITEM:
			v = POP();

			PyObject_Print(v);
			Py_DECREF(v);
			break;

		case PRINT_NEWLINE:
			printf("\n");
			break;

		case RETURN_VALUE:
			retval = POP();
			why = WHY_RETURN;
			break;

		case POP_BLOCK:
			{
				PyTryBlock *b = PyFrame_BlockPop(f);
				while (STACK_LEVEL() > b->b_level) {
					v = POP();
					Py_DECREF(v);
				}
			}
			break;

		case STORE_NAME:
			w = GETITEM(names, oparg);
			v = POP();
			if ((x = f->f_locals) == NULL) {
				/* ERROR */
				printf("STORE_NAME ERROR\n");
				break;
			}
			err = PyDict_SetItem(x, w, v);
			Py_DECREF(v);
			break;

		case LOAD_NAME:
			w = GETITEM(names, oparg);
			if ((x = f->f_locals) == NULL) {
				/* ERROR */
				printf("LOAD_NAME ERROR\n");
				break;
			}
			x = PyDict_GetItem(x, w);
			if (x == NULL) {
				x = PyDict_GetItem(f->f_globals, w);
				if (x == NULL) {
					x = PyDict_GetItem(f->f_builtins, w);
					if (x == NULL) {
						printf("can't find %s", ((PyStringObject *)w)->ob_sval);
						/* format_exc_check_arg */
						break;
					}
				}
			}
			Py_INCREF(x);
			PUSH(x);
			break;
			
		case LOAD_GLOBAL:
			w = GETITEM(names, oparg);
			if (PyString_CheckExact(w)) {
				/* Inline the PyDict_GetItem() calls.
				   WARNING: this is an extreme speed hack.
				   Do not try this at home. */
				long hash = ((PyStringObject *)w)->ob_shash;
				if (hash != -1) {
					PyDictObject *d;
					d = (PyDictObject *)(f->f_globals);
					x = d->ma_lookup(d, w, hash)->me_value;
					if (x != NULL) {
						Py_INCREF(x);
						PUSH(x);
						continue;
					}
					d = (PyDictObject *)(f->f_builtins);
					x = d->ma_lookup(d, w, hash)->me_value;
					if (x != NULL) {
						Py_INCREF(x);
						PUSH(x);
						continue;
					}
					goto load_global_error;
				}
			}
			/* This is the un-inlined version of the code above */
			x = PyDict_GetItem(f->f_globals, w);
			if (x == NULL) {
				x = PyDict_GetItem(f->f_builtins, w);
				if (x == NULL) {
				  load_global_error:
					printf("LOAD_GLOBAL ERROR %s", ((PyStringObject *)w)->ob_sval);
					break;
				}
			}
			Py_INCREF(x);
			PUSH(x);
			break;

		case LOAD_ATTR:
			w = GETITEM(names, oparg);
			v = TOP();
			x = PyObject_GetAttr(v, w);
			Py_DECREF(v);
			SET_TOP(x);
			if (x != NULL) continue;
			break;

		case IMPORT_NAME:
			w = GETITEM(names, oparg);
			x = PyDict_GetItemString(f->f_builtins, "__import__");
			if (x == NULL) {
				printf("__import__ not found");
				break;
			}
			u = TOP();
			w = Py_BuildValue("(O)", w);
			Py_DECREF(u);
			if (w == NULL) {
				u = POP();
				x = NULL;
				break;
			}
			x = PyEval_CallObject(x, w);
			Py_DECREF(w);
			SET_TOP(x);
			if (x != NULL) continue;
			break;

		case JUMP_FORWARD:
			JUMPBY(oparg);
			goto fast_next_opcode;

		PREDICTED_WITH_ARG(JUMP_IF_FALSE);
		case JUMP_IF_FALSE:
			w = TOP();
			if (w == Py_True) {
				PREDICT(POP_TOP);
				goto fast_next_opcode;
			}
			if (w == Py_False) {
				JUMPBY(oparg);
				goto fast_next_opcode;
			}
			err = PyObject_IsTrue(w);
			if (err > 0)
				err = 0;
			else if (err == 0)
				JUMPBY(oparg);
			else
				break;
			continue;
			
		case JUMP_ABSOLUTE:
			JUMPTO(oparg);
			continue;

		case SETUP_LOOP:
			PyFrame_BlockSetup(f, opcode, INSTR_OFFSET() + oparg, STACK_LEVEL());
			continue;

		case CALL_FUNCTION:
			x = call_function(&stack_pointer, oparg);
			PUSH(x);
			if (x != NULL)
				continue;
			break;
			
		case MAKE_FUNCTION:
			v = POP(); /* code object */
			x = PyFunction_New(v, f->f_globals);
			Py_DECREF(v);
			/* XXX Maybe this should be a separate opcode? */
			if (x != NULL && oparg > 0) {
				v = PyTuple_New(oparg);
				if (v == NULL) {
					Py_DECREF(x);
					x = NULL;
			break;
				}
				while (--oparg >= 0) {
					w = POP();
					PyTuple_SET_ITEM(v, oparg, w);
				}
				err = PyFunction_SetDefaults(x, v);
				Py_DECREF(v);
			}
			PUSH(x);
			break;
			
		case SET_LINENO:
			break;

		default:
			printf("opcode: %d\n", opcode);
			Py_FatalError("unknown opcode");
		} /* switch */

	  on_error:
		
		if (why == WHY_NOT) {
			if (err == 0 && x != NULL) {
					continue; /* Normal, fast path */
			}
			why = WHY_EXCEPTION;
			x = Py_None;
			err = 0;
		}

		/* End the loop if we still have an error (or return) */

		if (why != WHY_NOT)
			break;

	} /* main loop */

	if (why != WHY_YIELD) {
		/* Pop remaining stack entries -- but when yielding */
		while (!EMPTY()) {
			v = POP();
			Py_XDECREF(v);
		}
	}

	if (why != WHY_RETURN && why != WHY_YIELD)
		retval = NULL;

	/* pop frame */
	--tstate->recursion_depth;
	tstate->frame = f->f_back;

	return retval;
}}

PyObject *
PyEval_EvalCodeEx(PyCodeObject *co, PyObject *globals, PyObject *locals,
	   PyObject **args, int argcount, PyObject **kws, int kwcount,
	   PyObject **defs, int defcount, PyObject *closure)
{
	register PyFrameObject *f;
	register PyObject *retval = NULL;
	PyThreadState *tstate = PyThreadState_GET();

	if (globals == NULL) {
		/* ERROR */
		printf("PyEval_EvalCodeEx: NULL globals\n");
		return NULL;
	}

	f = PyFrame_New(tstate, co, globals, locals);
	if (f == NULL) {
		return NULL;
	}

	retval = eval_frame(f);

	++tstate->recursion_depth;
	Py_DECREF(f);
	--tstate->recursion_depth;
	return retval;
}

PyFrameObject *
PyEval_GetFrame(void)
{
	PyThreadState *tstate = PyThreadState_Get();
	return _PyThreadState_GetFrame(tstate);
}

PyObject *
PyEval_GetBuiltins(void)
{
	PyFrameObject *current_frame = PyEval_GetFrame();
	if (current_frame == NULL) {
		return PyThreadState_Get()->interp->builtins;
	}
	else {
		return current_frame->f_builtins;
	}
}

PyObject *
PyEval_CallObject(PyObject *func, PyObject *arg)
{
	return PyEval_CallObjectWithKeywords(func, arg, (PyObject *)NULL);
}

PyObject *
PyEval_CallObjectWithKeywords(PyObject *func, PyObject *arg, PyObject *kw)
{
	PyObject *result;

	if (arg == NULL)
		arg = PyTuple_New(0);
	else if (!PyTuple_Check(arg)) {
		printf("argument list must be a tuple");
		return NULL;
	}
	else
		Py_INCREF(arg);

	if (kw != NULL && !PyDict_Check(kw)) {
		printf("keyword list must be a dictionary");
		Py_DECREF(arg);
		return NULL;
	}

	result = PyObject_Call(func, arg, kw);
	Py_DECREF(arg);
	return result;
}

#define EXT_POP(STACK_POINTER) (*--(STACK_POINTER))

static void
err_args(PyObject *func, int flags, int nargs)
{
	/* TO DO */
}

/* The fast_function() function optimize calls for which no argument
   tuple is necessary; the objects are passed directly from the stack.
   For the simplest case -- a function that takes only positional
   arguments and is called with only positional arguments -- it
   inlines the most primitive frame setup code from
   PyEval_EvalCodeEx(), which vastly reduces the checks that must be
   done before evaluating the frame.
*/

static PyObject *
fast_function(PyObject *func, PyObject ***pp_stack, int n, int na, int nk)
{
	PyCodeObject *co = (PyCodeObject *)PyFunction_GET_CODE(func);
	PyObject *globals = PyFunction_GET_GLOBALS(func);
	PyObject *argdefs = PyFunction_GET_DEFAULTS(func);
	PyObject **d = NULL;
	int nd = 0;

	if (argdefs == NULL && co->co_argcount == n && nk==0 &&
	    co->co_flags == (CO_OPTIMIZED | CO_NEWLOCALS | CO_NOFREE)) {
		PyFrameObject *f;
		PyObject *retval = NULL;
		PyThreadState *tstate = PyThreadState_GET();
		PyObject **fastlocals, **stack;
		int i;

		/* XXX Perhaps we should create a specialized
		   PyFrame_New() that doesn't take locals, but does
		   take builtins without sanity checking them.
		*/
		f = PyFrame_New(tstate, co, globals, NULL);
		if (f == NULL)
			return NULL;

		fastlocals = f->f_localsplus;
		stack = (*pp_stack) - n;

		for (i = 0; i < n; i++) {
			Py_INCREF(*stack);
			fastlocals[i] = *stack++;
		}
		retval = eval_frame(f);
		++tstate->recursion_depth;
		Py_DECREF(f);
		--tstate->recursion_depth;
		return retval;
	}
	if (argdefs != NULL) {
		d = &PyTuple_GET_ITEM(argdefs, 0);
		nd = ((PyTupleObject *)argdefs)->ob_size;
	}
	return PyEval_EvalCodeEx(co, globals,
				 (PyObject *)NULL, (*pp_stack)-n, na,
				 (*pp_stack)-2*nk, nk, d, nd,
				 PyFunction_GET_CLOSURE(func));
}

static PyObject *
update_keyword_args(PyObject *orig_kwdict, int nk, PyObject ***pp_stack,
                    PyObject *func)
{
	PyObject *kwdict = NULL;
	if (orig_kwdict == NULL)
		kwdict = PyDict_New();
	else {
		kwdict = PyDict_Copy(orig_kwdict);
		Py_DECREF(orig_kwdict);
	}
	if (kwdict == NULL)
		return NULL;
	while (--nk >= 0) {
		int err;
		PyObject *value = EXT_POP(*pp_stack);
		PyObject *key = EXT_POP(*pp_stack);
		if (PyDict_GetItem(kwdict, key) != NULL) {
			/* ERROR */
			Py_DECREF(key);
			Py_DECREF(value);
			Py_DECREF(kwdict);
			return NULL;
		}
		err = PyDict_SetItem(kwdict, key, value);
		Py_DECREF(key);
		Py_DECREF(value);
		if (err) {
			Py_DECREF(kwdict);
			return NULL;
		}
	}
	return kwdict;
}

static PyObject *
load_args(PyObject ***pp_stack, int na)
{
	PyObject *args = PyTuple_New(na);
	PyObject *w;

	if (args == NULL)
		return NULL;
	while (--na >= 0) {
		w = EXT_POP(*pp_stack);
		PyTuple_SET_ITEM(args, na, w);
	}
	return args;
}

static PyObject *
do_call(PyObject *func, PyObject ***pp_stack, int na, int nk)
{
	PyObject *callargs = NULL;
	PyObject *kwdict = NULL;
	PyObject *result = NULL;

	if (nk > 0) {
		kwdict = update_keyword_args(NULL, nk, pp_stack, func);
		if (kwdict == NULL)
			goto call_fail;
	}
	callargs = load_args(pp_stack, na);
	if (callargs == NULL)
		goto call_fail;
	result = PyObject_Call(func, callargs, kwdict);
 call_fail:
	Py_XDECREF(callargs);
	Py_XDECREF(kwdict);
	return result;
}

static PyObject *
call_function(PyObject ***pp_stack, int oparg)
{
	int na = oparg & 0xff;
	int nk = (oparg>>8) & 0xff;
	int n = na + 2 * nk;
	PyObject **pfunc = (*pp_stack) - n - 1;
	PyObject *func = *pfunc;
	PyObject *x, *w;

	/* Always dispatch PyCFunction first, because these are
	   presumed to be the most frequent callable object.
	*/
	if (PyCFunction_Check(func) && nk == 0) {
		int flags = PyCFunction_GET_FLAGS(func);
		if (flags & (METH_NOARGS | METH_O)) {
			PyCFunction meth = PyCFunction_GET_FUNCTION(func);
			PyObject *self = PyCFunction_GET_SELF(func);
			if (flags & METH_NOARGS && na == 0) 
				x = (*meth)(self, NULL);
			else if (flags & METH_O && na == 1) {
				PyObject *arg = EXT_POP(*pp_stack);
				x = (*meth)(self, arg);
				Py_DECREF(arg);
			}
			else {
				err_args(func, flags, na);
				x = NULL;
			}
		}
		else {
			PyObject *callargs;
			callargs = load_args(pp_stack, na);
			x = PyCFunction_Call(func, callargs, NULL);
			Py_XDECREF(callargs); 
		} 
	} else {
		if (PyMethod_Check(func) && PyMethod_GET_SELF(func) != NULL) {
			/* optimize access to bound methods */
			PyObject *self = PyMethod_GET_SELF(func);
			Py_INCREF(self);
			func = PyMethod_GET_FUNCTION(func);
			Py_INCREF(func);
			Py_DECREF(*pfunc);
			*pfunc = self;
			na++;
			n++;
		} else
			Py_INCREF(func);
		if (PyFunction_Check(func))
			x = fast_function(func, pp_stack, n, na, nk);
		else 
			x = do_call(func, pp_stack, na, nk);
		Py_DECREF(func);
	}
	
	/* What does this do? */
	while ((*pp_stack) > pfunc) {
		w = EXT_POP(*pp_stack);
		Py_DECREF(w);
	}
	return x;
}

int
_PyEval_SliceIndex(PyObject *v, int *pi)
{
        if (v != NULL) {
                long x;
		x = PyInt_AS_LONG(v);
                *pi = x;
        }
        return 1;
}


static int
assign_slice(PyObject *u, PyObject *v, PyObject *w, PyObject *x)
        /* u[v:w] = x */
{
        int ilow = 0, ihigh = 1<<31;
        if (!_PyEval_SliceIndex(v, &ilow))
                return -1;
        if (!_PyEval_SliceIndex(w, &ihigh))
                return -1;
//        if (x == NULL)
//               return PySequence_DelSlice(u, ilow, ihigh);
//        else
                return PySequence_SetSlice(u, ilow, ihigh, x);
}
