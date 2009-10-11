import pyvga

def splashup():
	pyvga.exittext()
	pyvga.framebuffer[:0xFA00] = pyvga.splashscreen[:0xFA00]
	pyvga.cleartext()

def splashdown():
	pyvga.entertext()

splashup()

########################################
import isr
import py8042
import keyb

tb = pyvga.textbuffer

def prch(ch, fmt):
	print ch
	tb[0] = ch; tb[1] = fmt

def kbd_isr():
	while py8042.more_chars():
	    ch = keyb.translate_scancode(py8042.get_scancode())
	    if ch:
		prch(ch, '\015')

squeaks = 0
def clk_isr():
	tb[158] = '/-\|'[isr.ticker & 3]
	while py8042.more_squeaks():
		print "mouse: %x %d %d" % py8042.get_squeak()
		global squeaks
		squeaks = squeaks + 1
		
isr.setvec(clk_isr, kbd_isr)

########################################

while (isr.ticker < 40) and (squeaks < 1):
	pass

splashdown()
print 'Press a key'

while 1:
	pass
