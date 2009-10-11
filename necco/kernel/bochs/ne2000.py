import ports

ne_data  = 0x10
ne_reset = 0x1F

PKTSZ    = 3 * 512
RBUF     = 16 * 1024 + PKTSZ
RBUFEND  = 32 * 1024

### DS8390 ETHERNET CHIP

DS_PGSIZE  = 256

ds_cmd     = 0x00
DSCM_STOP         = 0x01
DSCM_START        = 0x02
DSCM_RREAD        = 0x08
DSCM_NODMA        = 0x20
DSCM_PG0          = 0x00
DSCM_PG1          = 0x40

ds0_pstart = 0x01
ds0_pstop  = 0x02
ds0_bnry   = 0x03

ds0_isr    = 0x07
DSIS_RDC          = 0x40
DSIS_RESET        = 0x80

ds0_rsar0  = 0x08        # remote start addr low
ds0_rsar1  = 0x09        #                   high
ds0_rbcr0  = 0x0A        # remote byte count low
ds0_rbcr1  = 0x0B        #                   high

ds0_rcr    = 0x0C
DSRC_AB           = 0x04
DSRC_MON          = 0x20

ds0_tcr    = 0x0D
DSTC_LB0          = 0x02

ds0_dcr    = 0x0E
DSDC_WTS          = 0x01
DSDC_BMS          = 0x08
DSDC_FT1          = 0x40

ds0_imr    = 0x0F

ds1_par0 = ds0_pstart
ds1_mar0 = ds0_rsar0
ds1_curr = ds0_isr

###

def fetch(adapter_addr, addr, length):

    cmd = ports.inb(adapter_addr + ds_cmd)

    ports.outb(adapter_addr + ds_cmd, DSCM_NODMA | DSCM_PG0 | DSCM_START)

    ports.outb(adapter_addr + ds0_isr, DSIS_RDC)
    ports.outb(adapter_addr + ds0_rbcr0, length)
    ports.outb(adapter_addr + ds0_rbcr1, length >> 8)
    ports.outb(adapter_addr + ds0_rsar0, addr)
    ports.outb(adapter_addr + ds0_rsar1, addr >> 8)

    ports.outb(adapter_addr + ds_cmd, DSCM_RREAD | DSCM_PG0 | DSCM_START)

    #buffer = ports.repinsw(adapter_addr + ne_data, length / 2)
    #if length & 1:
    #    buffer += ports.inb(adapter_addr + ne_data)

    buffer = ports.repinsb(adapter_addr + ne_data, length)

    while not (ports.inb(adapter_addr + ds0_isr) & DSIS_RDC):
        pass

    ports.outb(adapter_addr + ds0_isr, DSIS_RDC)
    ports.outb(adapter_addr + ds_cmd, cmd)

    return buffer

def pause():
    i = 50000
    while i > 0:
        i = i - 1


def init_device(adapter_addr):

    print "Initializing network device..."

    print "Resetting device"

    val = ports.inb(adapter_addr + ne_reset)
    pause()
    ports.outb(adapter_addr + ne_reset, val)
    ports.outb(adapter_addr + ds_cmd, DSCM_STOP | DSCM_NODMA)

    i = 20000
    while i > 0:
        i = i - 1
        if ports.inb(adapter_addr + ds0_isr) & DSIS_RESET:
            break

    if not i:
        print "FAILURE: reset timed out"
        return

    print "Device successfully reset"

    ports.outb(adapter_addr + ds0_isr, 0xFF);

    ports.outb(adapter_addr + ds0_dcr, DSDC_WTS | DSDC_BMS | DSDC_FT1)

    ports.outb(adapter_addr + ds_cmd, DSCM_NODMA | DSCM_PG0 | DSCM_STOP)

    pause()

    if ports.inb(adapter_addr + ds_cmd) != (DSCM_NODMA | DSCM_PG0 | DSCM_STOP):
        print "Initialization failed"
        return

    ports.outb(adapter_addr + ds0_tcr, 0)
    ports.outb(adapter_addr + ds0_rcr, DSRC_MON)
    ports.outb(adapter_addr + ds0_pstart, RBUF/DS_PGSIZE)
    ports.outb(adapter_addr + ds0_pstop, RBUFEND/DS_PGSIZE)
    ports.outb(adapter_addr + ds0_bnry, RBUF/DS_PGSIZE)
    ports.outb(adapter_addr + ds0_imr, 0)
    ports.outb(adapter_addr + ds0_isr, 0)
    ports.outb(adapter_addr + ds_cmd, DSCM_NODMA | DSCM_PG1 | DSCM_STOP)
    ports.outb(adapter_addr + ds1_curr, RBUF/DS_PGSIZE)
    ports.outb(adapter_addr + ds_cmd, DSCM_NODMA | DSCM_PG0 | DSCM_STOP)

    data = fetch(adapter_addr, 0, 16)
    global mac_address
    mac_address = data[0], data[2], data[4], data[6], data[8], data[10]

    print mac_address

    print "Device successfully initialized!"

def configure_device(adapter_addr, mac_address):

    print "Configuring network device..."

    ports.outb(adapter_addr + ds_cmd, DSCM_NODMA | DSCM_PG0 | DSCM_STOP)

    ports.outb(adapter_addr + ds0_dcr, DSDC_WTS | DSDC_BMS | DSDC_FT1)

    ports.outb(adapter_addr + ds0_rbcr0, 0)
    ports.outb(adapter_addr + ds0_rbcr1, 0)

    ports.outb(adapter_addr + ds0_rcr, DSRC_MON)

    ports.outb(adapter_addr + ds0_tcr, DSTC_LB0)

    ports.outb(adapter_addr + ds0_pstart, RBUF/DS_PGSIZE)
    ports.outb(adapter_addr + ds0_pstop, RBUFEND/DS_PGSIZE)
    ports.outb(adapter_addr + ds0_bnry, RBUF/DS_PGSIZE)

    ports.outb(adapter_addr + ds0_isr, 0xFF)

    ports.outb(adapter_addr + ds0_imr, 0XFF)

    ports.outb(adapter_addr + ds_cmd, DSCM_NODMA | DSCM_PG1 | DSCM_STOP)

    for i, b in enumerate(mac_address):
        ports.outb(adapter_addr + ds1_par0 + i, b)

    for i in range(8):
        ports.outb(adapter_addr + ds1_mar0 + i, 0xFF)

    ports.outb(adapter_addr + ds1_curr, RBUF/DS_PGSIZE)

    ports.outb(adapter_addr + ds_cmd, DSCM_NODMA | DSCM_PG0 | DSCM_START)

    ports.outb(adapter_addr + ds0_tcr, 0)

    ports.outb(adapter_addr + ds0_rcr, DSRC_AB)

    print "Device successfully configured!"


print "NE2000 Driver Experimentation"

init_device(0x300)
configure_device(0x300, mac_address)
