#include "Python.h"

extern _end;
static void *curr = &_end;

void *
malloc(size_t size)
{
#ifdef VERBOSE_MALLOC
	print("malloc ");
	print_hex(size);
	print(" @ ");
	print_hex(curr);
	print("   ");
#endif

	void *prev = curr;
	curr += size;
	return (void *) prev;
}
