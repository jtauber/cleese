import ports

def seconds(): return ports.querypair(0x70, 0)
def minutes(): return ports.querypair(0x70, 2)
def hours():   return ports.querypair(0x70, 4)
