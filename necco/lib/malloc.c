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

void
free(void *mem)
{
  /* @@@ do nothing for now */
}

void *
realloc(void *mem, size_t newsize)
{
  /* @@@ need to know the previous size */
  void *new = malloc(newsize);
  memcpy(new, mem, newsize); /* too much, but this is a kludge */
  return new;
}
