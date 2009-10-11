#somehow this initializes pyvga twice?# import pyvga

""" pure python text rendering:
    still slow on my bochs, but runs fine on hardware """

pal 	= '\037\067\067.\067...\067'
fb	= ''
font	= ''

def setctx(_fb, _font):
	global fb, font
	fb = _fb
	font = _font

def render(fbo, glyph):
	off = glyph << 5
	loff = off + 16
	while off < loff:
		scan = ord(font[off])
		if scan:
			hisc = scan >> 4
			fb[fbo + 0] = pal[hisc & 0x8]
			fb[fbo + 1] = pal[hisc & 0x4]
			fb[fbo + 2] = pal[hisc & 0x2]
			fb[fbo + 3] = pal[hisc & 0x1]
			fb[fbo + 4] = pal[scan & 0x8]
			fb[fbo + 5] = pal[scan & 0x4]
			fb[fbo + 6] = pal[scan & 0x2]
			fb[fbo + 7] = pal[scan & 0x1]
		else:   fb[fbo:fbo+8] = '\037\037\037\037\037\037\037\037'
		fbo = fbo + 320
		off = off + 1

def write(x, y, str):
	fbo = (y << 8) + (y << 6) + x	# y * 320 + x

	i = 0
	last = len(str)
	while i < last:
		render(fbo, ord(str[i]))
		fbo = fbo + 8
		i = i + 1
