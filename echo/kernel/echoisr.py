def more_chars():
    return (inb(0x64) & 0x01)

def get_scancode():
    while not more_chars():
        pass
    return inb(0x60)

def translate_scancode(scancode):
    if (scancode & 0x80): # high bit set (key release)
        return None
    return "?E1234567890-=BTqwertyuiop[]N^asdfghjkl;'`Z\\zxcvbnm,./SXXXXXXXXX"[scancode & 0x3F]

tb = textbuffer()
ticks = 0

def kbd_isr():
	while more_chars():
	    ch = translate_scancode(get_scancode())
	    if ch:
		print ch
		tb[0] = ch; tb[1] = '\015'

def clk_isr():
	tb[158] = '01234567'[ticks & 7]
	framebuffer[0x800] = '\000\020\360\377'[ticks & 3]

setvec()

while 1:
	ticks = ticks + 1
	if not (ticks & 0xfff):
		if (ticks & 0x1000): videomode(1)
		else:		     videomode(0)
