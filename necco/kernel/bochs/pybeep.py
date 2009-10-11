import ports
inb  = ports.inb
outb = ports.outb

def on(freq):
	if not freq:
		off()
		return
	outb(freq,      0x42)
	outb(freq >> 8, 0x42)
	outb(inb(0x61) | 0x03, 0x61)

def off():
	outb(inb(0x61) & 0xFC, 0x61)
