import ports

# simple synchronous serial port
# (enough to debug with, anyway)

def ready():
	return (ports.inb(0x3FD) & 0x20)

def send(ch):
	while not ready(): pass
	ports.outb(ord(ch), 0x3F8)

ports.outb(0x80,0x3FB)
ports.outb(0x01,0x3F8)	# baud rate:
ports.outb(0x00,0x3F9)	# 115200
ports.outb(0x03,0x3FB)
ports.outb(0x03,0x3FC)
ports.outb(0x00,0x3F9)
