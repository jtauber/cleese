
#include "stdarg.h"

extern unsigned char in(unsigned short);
extern void out(unsigned short, unsigned char);

#define VIDMEM              0xB8000

#define CRT_CTRL_PORT       0x3D4
#define CRT_DATA_PORT       0x3D5
#define CRT_CURSOR_LOC_HIGH 0x0E
#define CRT_CURSOR_LOC_LOW  0x0F

#define NUM_COLS 80
#define NUM_ROWS 25
#define SCREEN_SIZE (NUM_COLS * NUM_ROWS)

#define SCREEN_ATTR 0x0F

static int row, col;

void
clear_screen()
{
	unsigned char *vidmem = (unsigned char *) VIDMEM;
	int loop;

	for (loop = 0; loop < SCREEN_SIZE; loop++) {
		*vidmem++ = 0x20;
		*vidmem++ = SCREEN_ATTR;
	}
}

void
init_screen()
{
	row = col = 0;
	clear_screen();
}

static void
update_cursor()
{
	unsigned short offset;

	offset = (row * NUM_COLS) + col;

	out(CRT_CTRL_PORT, CRT_CURSOR_LOC_HIGH);
	out(CRT_DATA_PORT, (offset >> 8) & 0xFF);

	out(CRT_CTRL_PORT, CRT_CURSOR_LOC_LOW);
	out(CRT_DATA_PORT, offset & 0xFF);
}

static void
scroll_screen()
{
	unsigned short* v;
	int i;
	int n = SCREEN_SIZE;
	
	for (v = (unsigned short*) VIDMEM, i = 0; i < n; i++ ) {
		*v = *(v + NUM_COLS);
		++v;
	}

	for (v = (unsigned short*) VIDMEM + n, i = 0; i < NUM_COLS; i++) {
		*v++ = SCREEN_ATTR & (0x20 << 8);
	}
}

static void
new_line()
{
	++row;
	col = 0;
	if (row == NUM_ROWS) {
		scroll_screen();
		row = NUM_ROWS - 1;
	}
}

static void
clear_to_EOL()
{
	int loop;
	unsigned char *v = (unsigned char *) (VIDMEM + row * NUM_COLS * 2 + col * 2);

	for (loop = col; loop < NUM_COLS; loop++) {
		*v++ = ' ';
		*v++ = SCREEN_ATTR;
	}
}

void
print_char(int c)
{
	unsigned char *v = (unsigned char *) (VIDMEM + row * NUM_COLS * 2 + col * 2);

	switch(c) {
	case '\n':
		clear_to_EOL();
		new_line();
		break;
	default:
		*v++ = (unsigned char) c;
		*v   = SCREEN_ATTR;
		
		if (col < NUM_COLS - 1)
			++col;
		else
			new_line();
	}
}

static void
print_string(const char *s)
{
	while (*s != '\0') {
		print_char(*s++);
	}
	update_cursor();
}

static void
format_d(char* buf, int val)
{
	char stack[16];
	int top = 0;
	int negative;
	unsigned uval;

	if (val < 0) {
		negative = 1;
		uval = -val;
	} else {
		negative = 0;
		uval = val;
	}

	do {
		int digit = uval %10;
		stack[top++] = digit + '0';
		uval /= 10;
	}
	while (uval > 0);

	if (negative) {
		*buf++ = '-';
	}

	do {
		*buf++ = stack[--top];
	}
	while (top > 0);

	*buf = '\0';
}

static
void format_x(char* buf, int val)
{
	int i;
	for (i = 28; i >=0; i -= 4) {
		int x = (val >> i) & 0xf;
		*buf++ = "0123456789ABCDEF"[x];
	}
	*buf = '\0';
}

#define	SCREEN		((char *)-1)

static
char *p_string(char *d, const char *s)
{
	if(d == SCREEN)	{
		print_string(s);
		return SCREEN;
	}
	while (*s != '\0') {
		*d++ = *s++;
	}
	return d;
}

void vsprintf(char *dst, const char *fmt, va_list args)
{
	char buf[64];
	int ival;
	const char* sval;

	while (*fmt != '\0') {
		switch (*fmt) {
		case '%':
			++fmt;
			switch (*fmt) {
			case 'd':
				ival = va_arg(args, int);
				format_d(buf, ival);
				dst = p_string(dst, buf);
				break;
			case 'x':
				ival = va_arg(args, int);
				format_x(buf, ival);
				dst = p_string(dst, buf);
				break;
			case 's':
				sval = va_arg(args, char *);
				dst = p_string(dst, sval);
				break;
			default:
				buf[0] = *fmt;
				buf[1] = '\0';
				dst = p_string(dst, buf);
				break;
			}
			break;
		default:
			buf[0] = *fmt;
			buf[1] = '\0';
			dst = p_string(dst, buf);
			break;
		}

		fmt++;
	}
	if(dst == SCREEN)
		update_cursor();
	else
		*dst++ = '\0';
}

void
sprintf(char *dst, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsprintf(dst, fmt, args);
	va_end(args);
}

void
printf(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsprintf(SCREEN, fmt, args);
	va_end(args);
}
