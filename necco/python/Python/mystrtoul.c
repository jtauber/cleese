
#include "Python.h"

#include "ctype.h"

unsigned long
PyOS_strtoul(register char *str, char **ptr, int base)
{
    register unsigned long	result;	/* return value of the function */
    register int		c;	/* current input character */
    register unsigned long	temp;	/* used in overflow testing */
    int				ovf;	/* true if overflow occurred */

    result = 0;
    ovf = 0;

/* catch silly bases */
    if (base != 0 && (base < 2 || base > 36))
    {
	if (ptr)
	    *ptr = str;
	return 0;
    }

/* skip leading white space */
    while (*str && isspace(Py_CHARMASK(*str)))
	str++;

/* check for leading 0 or 0x for auto-base or base 16 */
    switch (base)
    {
    case 0:		/* look for leading 0, 0x or 0X */
	if (*str == '0')
	{
	    str++;
	    if (*str == 'x' || *str == 'X')
	    {
		str++;
		base = 16;
	    }
	    else
		base = 8;
	}
	else
	    base = 10;
	break;

    case 16:	/* skip leading 0x or 0X */
	if (*str == '0' && (*(str+1) == 'x' || *(str+1) == 'X'))
	    str += 2;
	break;
    }

/* do the conversion */
    while ((c = Py_CHARMASK(*str)) != '\0')
    {
	if (isdigit(c) && c - '0' < base)
	    c -= '0';
	else
	{
	    if (isupper(c))
		c = tolower(c);
	    if (c >= 'a' && c <= 'z')
		c -= 'a' - 10;
	    else	/* non-"digit" character */
		break;
	    if (c >= base)	/* non-"digit" character */
		break;
	}
	temp = result;
	result = result * base + c;
#ifndef MPW
	if(base == 10) {
		if(((long)(result - c) / base != (long)temp))	/* overflow */
			ovf = 1;
	}
	else {
		if ((result - c) / base != temp)	/* overflow */
			ovf = 1;
	}
#endif
	str++;
    }

/* set pointer to point to the last character scanned */
    if (ptr)
	*ptr = str;
    if (ovf)
    {
	result = (unsigned long) ~0L;
	errno = ERANGE;
    }
    return result;
}

long
PyOS_strtol(char *str, char **ptr, int base)
{
	long result;
	char sign;
	
	while (*str && isspace(Py_CHARMASK(*str)))
		str++;
	
	sign = *str;
	if (sign == '+' || sign == '-')
		str++;
	
	result = (long) PyOS_strtoul(str, ptr, base);
	
	/* Signal overflow if the result appears negative,
	   except for the largest negative integer */
	if (result < 0 && !(sign == '-' && result == -result)) {
		errno = ERANGE;
		result = 0x7fffffff;
	}
	
	if (sign == '-')
		result = -result;
	
	return result;
}
