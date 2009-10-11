import ports

autosense = 1
foundxt = 0
_sensebuf = 0

# I can't seem to get the Windows Bochs to either (a) generate scancode 2 data,
# or (b) tell us that it's going to generate scancode 1 data, so we look at the
# first few scancodes, and guess.
#
# this probably fails miserably if the first keystrokes have a modifier,
# but I'm still hoping there's a better fix than autosensing.

def check_auto(scancode):
	global autosense, foundxt, _sensebuf
	if not _sensebuf:
		if scancode == 0xE0:
			foundxt = 0
			autosense = 0
			return scancode
		_sensebuf = scancode
		return 0
	if scancode == 0xF0:
		global _break
		_break = 1
		foundxt = 0
		autosense = 0
		return _sensebuf
	else:
		foundxt = 1
		autosense = 0
		return _sensebuf
			

""" Keyboard decode for PS/2 keyboard scan codes """

_ctl = 0
_alt = 0
_shift = 0

def translate_scancode(scancode):
    if autosense:
	scancode = check_auto(scancode)
    if scancode:
    	if foundxt: return translate_xt(scancode)
	else:	    return translate_at(scancode)

################################################################################

_break = 0

def break_at(scancode):
    global _break, _shift, _ctl, _alt
    if scancode == 0xF0:
	_break = 1
	return 1
    elif _break == 1:
	if   scancode == 0x11: _alt = 0
	elif scancode == 0x14: _ctl = 0
	elif scancode == 0x12: _shift = 0
	elif scancode == 0x59: _shift = 0
	_break = 0
	return 1
    elif scancode == 0x11: _alt = 1;   return 1
    elif scancode == 0x14: _ctl = 1;   return 1
    elif scancode == 0x12: _shift = 1; return 1
    elif scancode == 0x59: _shift = 1; return 1
    elif scancode == 0x58: _shift = not _shift; return 1
    else:
	return 0

shft_at = "_____________\t~______Q!___ZSAW@__CXDE$#__ VFTR%__NBHGY^___MJU*&__<KIO)(__>?L:P_-__\"_{+____\n}_|__"
nosh_at = "_____________\t`______q1___zsaw2__cxde43__ vftr5__nbhgy6___mju78__,kio09__./l;p--__'_[=____\n]_\\__"


def translate_at(scancode):
    if scancode == 0x71 and _alt == 1 and _ctl == 1:
	ports.outb(0xfe, 0x64) # reboot
    if scancode == 0xE0: return None		# FIXME

    if break_at(scancode) == 1:
	return None

    if scancode > 0x60: return None	# FIXME
    if _ctl == 1:	return None	# chr(ord(shifted[scancode]) & 0x1F)
    elif _shift == 1:	return shft_at[scancode]
    else:		return nosh_at[scancode]

################################################################################

shft_xt = "?\033!@#$%^&*()_+\010\tQWERTYUIOP{}\n^ASDFGHJKL:\"~Z|ZXCVBNM<>?SXX XXXXX"
nosh_xt = "?\0331234567890-=\010\tqwertyuiop[]\n^asdfghjkl;'`Z\\zxcvbnm,./SXX XXXXX"

def translate_xt(scancode):
    global _shift, _ctl, _alt
    if scancode == 0xE0 and _alt == 1 and _ctl == 1:
	ports.outb(0xfe, 0x64) # reboot

    if (scancode & 0x80): # high bit set (key release)
    	if scancode == 0x9D:   _ctl = 0;   return None
    	elif scancode == 0xAA: _shift = 0; return None
    	elif scancode == 0xB6: _shift = 0; return None
	elif scancode == 0xB8: _alt = 0;   return None
        return None

    if scancode == 0x1D:   _ctl = 1;   return None
    elif scancode == 0x2A: _shift = 1; return None
    elif scancode == 0x36: _shift = 1; return None
    elif scancode == 0x38: _alt = 1;   return None

    if scancode > 0x3F: return None	# FIXME
    if _ctl == 1:	return None	# chr(ord(shifted[scancode]) & 0x1F)
    elif _shift == 1:	return shft_xt[scancode]
    else:		return nosh_xt[scancode]
