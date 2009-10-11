/* Descriptors */
#ifndef Py_DESCROBJECT_H
#define Py_DESCROBJECT_H

/* ... */

#define PyDescr_IsData(d) ((d)->ob_type->tp_descr_set != NULL)

/* ... */

#endif /* !Py_DESCROBJECT_H */
