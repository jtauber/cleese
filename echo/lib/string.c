#include "sys/types.h"

int
strcmp(const char *s1, const char *s2)
{
	while (*s1++ == *s2) {
		if (*s2++ == '\0') {
			return(0);
		}
	}
	return((int)s1[-1] - (int)s2[0]);
	return(1);
}

size_t
strlen(const char *p)
{
	size_t x = 0;

	if (p == 0) {
		return(0);
	}

	while (*p++)
		++x;
	return(x);
}

/* see mem.s */
extern void bcopy(const void *, void *, size_t);

void *
memcpy(void *dest, const void *src, size_t cnt)
{
#ifdef VERBOSE_MEMCPY
	printf("memcpy %x from %x to %x\n", cnt, src, dest);
#endif

	bcopy(src, dest, cnt);
	return(dest);
}

void *
memset(void *dst, int c, size_t n)
{
	if (n != 0) {
		char  *d = dst;

		do {
			*d++ = c;
		} while (--n != 0);
	}
	return (dst);
}

int
bcmp(const void *s1, const void *s2, unsigned int n)
{
        const char *p = s1, *q = s2;

        while (n-- > 0) {
                if (*p++ != *q++) {
                        return((int)p[-1] - (int)q[-1]);
                }
        }
        return(0);
}

int
memcmp(const void *s1, const void *s2, size_t n)
{
	return(bcmp(s1, s2, n));
}
