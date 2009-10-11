/*
 * snprintf.c
 */
#include "stdarg.h"
#include "ctype.h"
#include "sys/types.h"

#define MORE_THAN_YOU_WANT	1<<30
#define MAX_STDOUT_CHARS	255
extern void print_string(const char *);

static char hexmap[] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

void va_snprintf(char *b, int l, const char *fmt, va_list pvar)
{
    int n, i;
    uint_t u;
    ulonglong_t ull;
    char *t;
    char d[10];
    char mod_l;
    char mod_ll;

    if (!fmt || !b || (l < 1))
	return;

    mod_l = mod_ll = 0;
    while (l && *fmt) {
	if (*fmt == '%') {
	    if (!(--l))
		break;
	  again:
	    fmt++;

	    switch (*fmt) {
	    case '.':		/* precision modifier */
		while(isdigit(fmt[1]))
			fmt++;
		goto again;

	    case 'l':		/* long modifier */
		if (!mod_l) {
		    mod_l = 1;
		} else if (!mod_ll) {
		    mod_l = 0;
		    mod_ll = 1;
		}
		goto again;

	    case 's':		/* string */
		t = va_arg(pvar, char *);
		while (l && *t)
		    *b++ = *t++, l--;
		break;

	    case 'c':		/* single character */
		*b++ = va_arg(pvar, char);
		l--;
		break;

	    case 'S':		/* uint32 as a short ... */
		if (l < 4) {
		    l = 0;
		    break;
		}
		u = va_arg(pvar, unsigned int);
		for (i = 3; i >= 0; i--) {
		    b[i] = hexmap[u & 0x0F];
		    u >>= 4;
		}
		b += 4;
		l -= 4;
		break;

	    case 'x':
	    case 'p':
		if (!mod_ll) {	/* 8 digit, unsigned 32-bit hex integer */
		    if (l < 8) {
			l = 0;
			break;
		    }
		    u = va_arg(pvar, unsigned int);
		    for (i = 7; i >= 0; i--) {
			b[i] = hexmap[u & 0x0F];
			u >>= 4;
		    }
		    b += 8;
		    l -= 8;
		} else if (mod_ll) {	/* 16 digit, unsigned 64-bit hex integer */
		    if (l < 16) {
			l = 0;
			break;
		    }
		    ull = va_arg(pvar, unsigned long long);
		    for (i = 15; i >= 0; i--) {
			b[i] = hexmap[ull & 0x0f];
			ull >>= 4;
		    }
		    b += 16;
		    l -= 16;
		}
		mod_l = mod_ll = 0;
		break;

	    case 'd':		/* signed integer */
		n = va_arg(pvar, int);
		if (n < 0) {
		    u = -n;
		    *b++ = '-';
		    if (!(--l))
			break;
		} else {
		    u = n;
		}
		goto u2;

	    case 'u':		/* unsigned integer */
		u = va_arg(pvar, unsigned int);
	      u2:
		i = 9;
		do {
		    d[i] = (u % 10) + '0';
		    u /= 10;
		    i--;
		}
		while (u && i >= 0);
		while (++i < 10) {
		    *b++ = d[i];
		    if (!(--l))
			break;
		}
		break;

	    case 'U':
		u = va_arg(pvar, unsigned int);
		i = 9;
		d[8] = d[7] = d[6] = ' ';
		do {
		    d[i] = (u % 10) + '0';
		    u /= 10;
		    i--;
		}
		while (u && i >= 0);
		i = 5;
		while (++i < 10) {
		    *b++ = d[i];
		    if (!(--l))
			break;
		}
		break;

	    case 'X':		/* 2 digit, unsigned 8bit hex int */
		if (l < 2) {
		    l = 0;
		    break;
		}
		n = va_arg(pvar, int);
		*b++ = hexmap[(n & 0xF0) >> 4];
		*b++ = hexmap[n & 0x0F];
		l -= 2;
		break;
	    default:
		*b++ = *fmt;
	    }
	} else {
	    *b++ = *fmt;
	    l--;
	}
	fmt++;
    }
    *b = 0;
}

void snprintf(char *str, int len, char *fmt, ...)
{
    va_list pvar;
    va_start(pvar, fmt);
    va_snprintf(str, len, fmt, pvar);
    va_end(pvar);
}

void
sprintf(char *dst, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    va_snprintf(dst, MORE_THAN_YOU_WANT, fmt, args);
    va_end(args);
}

void
printf(const char* fmt, ...)
{
    static char buf[MAX_STDOUT_CHARS];

    va_list args;
    va_start(args, fmt);
    va_snprintf(buf, MAX_STDOUT_CHARS, fmt, args);
    va_end(args);

    print_string(buf);
}
