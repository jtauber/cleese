#ifndef Py_PYMEM_H
#define Py_PYMEM_H

PyAPI_FUNC(void *) PyMem_Malloc(size_t);
PyAPI_FUNC(void *) PyMem_Realloc(void *, size_t);
PyAPI_FUNC(void) PyMem_Free(void *);

#define PyMem_MALLOC(n)     malloc((n) ? (n) : 1)
#define PyMem_REALLOC(p, n) realloc((p), (n) ? (n) : 1)

#define PyMem_NEW(type, n) ( (type *) PyMem_MALLOC((n) * sizeof(type)) )

#define PyMem_FREE(op) /* Nothing */

#define PyMem_RESIZE(p, type, n) \
        ( (p) = (type *) PyMem_REALLOC((p), (n) * sizeof(type)) )


#define PyMem_DEL /* Nothing */


#endif /* !Py_PYMEM_H */
