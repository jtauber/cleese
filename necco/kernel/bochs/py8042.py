import ports
inb  = ports.inb
outb = ports.outb

### KEYBOARD ###################################################################

def more_chars():
    return (inb(0x64) & 0x21) == 0x01

def get_scancode():
    while not more_chars():
        pass
    return inb(0x60)

### MOUSE ######################################################################

def more_squeaks():
	return (inb(0x64) & 0x21) == 0x21

def get_squeak():
	if not more_squeaks():
		return (0,0,0)

	fl = inb(0x60)
	while not more_squeaks(): pass
	dx = inb(0x60)
	while not more_squeaks(): pass
	dy = inb(0x60)
	if fl & 0x20: dy = -(256 + -dy)
	if fl & 0x10: dx = -(256 + -dx)
	return (fl & 0xCF, dx, dy)

### INIT #######################################################################

def wait_for_8042():
	while (inb(0x64) & 0x02):	# AUX_STATUS & IBF
		pass

def wait_for_ack():
	while not (inb(0x64) & 0x1): pass
	return inb(0x60)

def keyboard_kludge():
	""" [DL] my laptop will reboot if the mouse is used in XT mode """
	wait_for_8042()
	outb(0x60, 0x64)	# SET_FLAGS, AUX_COMMAND
	outb(0x05, 0x60)	# turn translation off, kbd interrupt only

def reboot():
	wait_for_8042()	
	outb(0xfe, 0x64)

keyboard_kludge()

wait_for_8042(); outb(0xa8, 0x64)	# AUX_ENABLE, AUX_COMMAND

wait_for_8042(); outb(0xd4, 0x64)	# AUX_WRITE_MOUSE, AUX_COMMAND
wait_for_8042(); outb(0xf4, 0x60)	# AUX_ENABLE_DEV, AUX_OUTPUT_PORT
wait_for_ack()
