/**
 * types.h
 */
#ifndef __TYPES_H__
#define __TYPES_H__

#define NULL ((void*)0)

typedef unsigned short      ushort_t;
typedef unsigned char       uchar_t;
typedef unsigned int        uint_t;
typedef unsigned long       ulong_t;
typedef unsigned long long  ulonglong_t;

typedef unsigned long       off_t;

#ifndef _SIZE_T
#  define _SIZE_T
typedef unsigned long       size_t;
#endif

#endif /* __TYPES_H__ */
