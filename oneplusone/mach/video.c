extern unsigned char in(unsigned short);
extern void out(unsigned short, unsigned char);

void
clrscr()
{
	unsigned char *vidmem = (unsigned char *)0xB8000;
	const long size = 80*25;
	long loop;

	// Clear visible video memory
	for (loop=0; loop<size; loop++) {
		*vidmem++ = 0;
		*vidmem++ = 0x0F;
	}

	// Set cursor position to 0,0
	out(0x3D4, 14);
	out(0x3D5, 0);
	out(0x3D4, 15);
	out(0x3D5, 0);
}

void
print(const char *_message)
{
	unsigned short offset;
	unsigned long i;
	unsigned char *vidmem = (unsigned char *)0xB8000;

	// Read cursor position
	out(0x3D4, 14);
	offset = in(0x3D5) << 8;
	out(0x3D4, 15);
	offset |= in(0x3D5);

	// Start at writing at cursor position
	vidmem += offset*2;

	// Continue until we reach null character
	i = 0;
	while (_message[i] != 0) {
		*vidmem = _message[i++];
		vidmem += 2;
	}

	// Set new cursor position
	offset += i;
	out(0x3D5, (unsigned char)(offset));
	out(0x3D4, 14);
	out(0x3D5, (unsigned char)(offset >> 8));
}

void
print_hex(unsigned long number)
{
	char buf[] = "0x00000000";
	char hex[] = "0123456789ABCDEF";
	unsigned int i, digit;

	for (i=9; i > 1; i--) {
		digit = number % 0x10;
		buf[i] = hex[digit];
		number /= 0x10;
		if (number == 0)
			break;
	}
	print(buf);
}
