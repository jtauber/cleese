#include "Python.h"

extern void _end;
static void *curr = &_end;

void *
malloc(size_t size)
{
#ifdef VERBOSE_MALLOC
	printf("malloc %x @ %x\n", size, curr);
#endif

	void *prev = curr;
	curr += size;
	return (void *) prev;
}
