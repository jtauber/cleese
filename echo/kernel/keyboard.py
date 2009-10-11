
def get_scancode():
    while not (inb(0x64) & 0x01):
        pass
    return inb(0x60)

def translate_scancode(scancode):
    if (scancode & 0x80): # high bit set (key release)
        return None
    return "?E1234567890-=BTqwertyuiop[]N^asdfghjkl;'`Z\\zxcvbnm,./SXXXXXXXXX"[scancode & 0x3F]

tb = textbuffer()
tb[0:22] = 'p\017r\017e\017s\017s\017 \017a\017 \017k\017e\017y\017'

while 1:
    ch = translate_scancode(get_scancode())
    if ch:
        print ch
    tb[0] = ch; tb[1] = '\015'  # can't use % or pack yet
