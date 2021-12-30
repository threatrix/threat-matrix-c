
#include <fjtag.H>

amd_scs_flash_driver::amd_scs_flash_driver(Bus *b, cfi_query_structure_t *c) :
      flash_driver(b, c)
{
 // autoselect
 b->write(0x555, 0xaa); // 0x00aa00aa ?!
 b->write(0x2aa, 0x55);
 b->write(0x555, 0x90);
 mid_ = b->read(0);
 cid_ = b->read(1);
 protected_ = b->read(2);
 reset();
 switch (mid_) {
  case 0x1:
    manufacturer_ = "AMD";
    break;
  case 0x4:
    manufacturer_ = "Fujitsu";
    break;
  default:
    manufacturer_ = "Unknown manufacturer";
    break;   
 }
 switch (cid_) {
  case 0x22D7:
    name_ = "Am29LV640D/Am29LV641D/Am29LV642D";
    break;
  case 0x225b:
    name_ = "Am29LV800B";
    break;
  case 0x22c4:
    name_ = "MBM29LV160TM";
    max_sector = 34;
    break;
  case 0x2249:
    name_ = "MBM29LV160BM";
    max_sector = 34;
    break;
  default:
    name_ = "Unknown device";
    break;   
 }
  if (c->identification_string.pri_vendor_tbl) {
    int addr = (int)c->identification_string.pri_vendor_tbl;
    b->write(0, CFI_CMD_READ_ARRAY1);
    b->write(CFI_CMD_QUERY_OFFSET, CFI_CMD_QUERY);
    int p = b->read(addr);
    int r = b->read(addr + 1);
    int i = b->read(addr + 2);
    if ((p != 'P') || (r != 'R') || (i != 'I'))
      goto out;
    char buf[1024], t[128];
    revision = (b->read(addr + 3) << 8) | b->read(addr + 4);
    sprintf(buf, " Revision : v%c.%c\n", revision >> 8, revision & 0xff);
    sprintf(t, " Address sensitive unlock required : %d\n", b->read(addr + 5));
    strcat(buf, t);
    sprintf(t, " Erase suspend : 0x%X\n", b->read(addr + 6));
    strcat(buf, t);
    sprintf(t, " Number of sectors in per group: 0x%X\n", b->read(addr + 7));
    strcat(buf, t);
    sprintf(t, " Sector temporary unprotection %s\n", b->read(addr + 8) ?
      "supported" : "not supported");
    strcat(buf, t);
    sprintf(t, " Sector protection algorithm : %d\n", b->read(addr + 9));
    strcat(buf, t);
    sprintf(t, " Dual operation %s\n", b->read(addr + 0xa) ?
      "supported" : "not supported");
    strcat(buf, t);
    sprintf(t, " Burst mode %s\n", b->read(addr + 0xb) ?
      "supported" : "not supported");
    strcat(buf, t);
    sprintf(t, " Page mode type %s\n", b->read(addr + 0xc) ?
      "supported" : "not supported");
    strcat(buf, t);
    int tmp = b->read(addr + 0xd);
    sprintf(t, " Vacc min supply : %d mV\n",
      ((tmp >> 4) & 0xF) * 1000 + (tmp & 0xF) * 100);
    strcat(buf, t);
    tmp = b->read(addr + 0xe);
    sprintf(t, " Vacc max supply : %d mV\n",
      ((tmp >> 4) & 0xF) * 1000 + (tmp & 0xF) * 100);
    strcat(buf, t);
    sprintf(t, " Write protect : %d\n", b->read(addr + 0xf));
    strcat(buf, t);
    sprintf(t, " Program suspend %s\n", b->read(addr + 0x10) ?
      "supported" : "not supported");
    strcat(buf, t);    
    pri_table_ = buf;
 }
out:
    reset();
}

int amd_scs_flash_driver::read(uint32_t adr)
{
 return bus->read(adr);
}

int amd_scs_flash_driver::burn(uint32_t adr, uint32_t data)
{
 bus->write(0x555, 0xaa);
 bus->write(0x2aa, 0x55);
 bus->write(0x555, 0xa0);
 bus->write(adr, data);
 if (status())
  message("Burn Time limit exceeded");
 reset();
 return 0;
}

// ????? 
static inline int make_val(unsigned char *b, int width)
{
 register int val = 0;
 for (register int i = 0; i < width; i++)
  val |= (b[i] << (i * 8));
 return val;
}

// bulk burn, using fast programm mode
// fully ugly and not portable, use a special class, not unsigned char *
int amd_scs_flash_driver::burn(uint32_t adr, unsigned char *data, uint32_t len)
{
 // fast mode on
 bus->write(0x555, 0xaa);
 bus->write(0x2aa, 0x55);
 bus->write(0x555, 0x20);
 register int val;
 for (register int i = 0; i < (int)len; i++) {
  val = make_val(&(data[i * (bus->width() / 8)]), (bus->width() / 8));
  bus->write(0x0, 0xa0);
  bus->write(adr + i, val);
  if (status())
    message("Burn Time limit exceeded");
  if (i && !(i % 1024))
    message("%d Kbytes burned", ((i + 1) / 1024));
 }
 // fast mode off
 bus->write(0x0, 0x90);
 bus->write(0x0, 0x0);
 reset();
 return 0;
}

int amd_scs_flash_driver::status(uint32_t adr)
{
 int ok = 0, t1, t2;
 uint32_t tmask = (1 << 6); // d6
 while (1) { // implement timeouts ?!
  t1 = (bus->read(adr) & 0xFF);
  t2 = (bus->read(adr) & 0xFF);
  if ((t1 & tmask) == (t2 & tmask)) // stop toggling
    break;
  ok = (t1 & (1 << 5)) | (t2 & (1 << 5));
  if (ok) // exceed time limit
    break;
 }
 return ok;
}

int amd_scs_flash_driver::chip_erase()
{
 bus->write(0x555, 0xaa);
 bus->write(0x2aa, 0x55);
 bus->write(0x555, 0x80);
 bus->write(0x555, 0xaa);
 bus->write(0x2aa, 0x55);
 bus->write(0x555, 0x10);
 message("Start erasing entire chip ...");
 if (status())
  message("Erase Time limit exceeded");
 else
  message("Erasing completed");
 reset();
 return 0;
}

static int sec2addr(int sector)
{
 switch(sector) {
  case 0 ... 2:
    return ((sector + 1) << 12);
  case 3 ... 5:
    return ((2 << 13) << (sector - 3));
//  case ....
  default:
    message("Invalid sector number %d", sector);
 }
  return 0;
}

int amd_scs_flash_driver::sector_erase(uint32_t sector)
{
 if ((int)sector > max_sector) {
  message("Invalid sector number %d", sector);
  return -1;
 }
 bus->write(0x555, 0xaa);
 bus->write(0x2aa, 0x55);
 bus->write(0x555, 0x80);
 bus->write(0x555, 0xaa);
 bus->write(0x2aa, 0x55);
 bus->write(sec2addr(sector), 0x30);
 message("Start erasing sector %d ...", sector);
 if (status())
  message("Erase Time limit exceeded");
 else
  message("Erasing completed"); 
 reset();
 return 0;
}

int amd_scs_flash_driver::protect(uint32_t sector)
{
 // not supported, need Vid (12v)
 message("protect: Not supported");
 return 0;
}

int amd_scs_flash_driver::unprotect(uint32_t sector)
{
 // not supported, need Vid (12v)
 message("unprotect: Not supported");
 return 0;
}

int amd_scs_flash_driver::isprotected(uint32_t sector)
{
 if ((int)sector > max_sector) {
  message("Invalid sector number %d", sector);
  return -1;
 }
 // issue autoselect
 bus->write(0x555, 0xaa);
 bus->write(0x2aa, 0x55);
 bus->write(0x555, 0x90);
 int addr = 0x002 | (sector << 12);
 int res = bus->read(addr);
 reset();
 return (res & 1);
}

int amd_scs_flash_driver::reset()
{
 bus->write(0, 0xf0);
 return 0;
}
