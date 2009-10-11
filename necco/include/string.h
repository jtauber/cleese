/**
 * string.h
 */
#ifndef ___STRING_H___
#define ___STRING_H___

#include "sys/types.h"

/*
 * True string functions
 */
char*  strcpy(char *, const char *);
int    strcmp(const char *, const char *);
size_t strlen(const char *);

/*
 * Memory routines
 */
void *memcpy(void *, const void *, size_t);
void *memset(void *, int, size_t);
int  *memcmp(const void *, const void *, size_t);
void *memmove(void *dest, const void *src, size_t length);

#endif   /* #ifndef ___STRING_H___ */
