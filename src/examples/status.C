
#include <ui.H>
#include <fjtag.H>
#include <string.h>
#include <stdio.h>

void update_parts_br()
{
 parts_br->clear();
 char b[256];
 for (int i = 0; i < (int)fjtag.parts.size(); i++) {
    sprintf(b, "@c@bPart N %d: %s", i, fjtag.parts[i]->name());
    parts_br->add(b, (void *)i);
    sprintf(b, "Manufacturer: %s", fjtag.parts[i]->manufacturer());
    parts_br->add(b, (void *)i);
    sprintf(b, "@sDevice Id: %s", fjtag.parts[i]->id->strval()->c_str());
    parts_br->add(b, (void *)i);
    sprintf(b, "Stepping: %s", fjtag.parts[i]->stepping());
    parts_br->add(b, (void *)i);
    parts_br->add("@-", (void *)-1);
 }
 update_pins_br();
}

void fill_signal_line(char *b, signal *s)
{
 char *dir = "X";
 bsbit *bit = 0;
 if (s->input && s->output) {
   dir = "Bidir";
   bit = s->input;
 } else if (s->input) {
   dir = "Input";
   bit = s->input;
 } else if (s->output) {
   dir = "Output";
   bit = s->output;
 }
 char b_[64];
 int val = 0;
 data_register *bsr = s->part->find_data_register("BSR");
 if (bsr && s->input)
   val = bsr->out->operator[](s->input->nbit());
// if (bit)
//   sprintf(b_, "[bit %d] - %d", bit->nbit(), val);
// else
//   strcpy(b_, "[no bit]");
 if (bit)
   sprintf(b_, " - %d", val);
 else
   strcpy(b_, " - X");
 sprintf(b, "%s signal %s %s", dir, s->name(), b_);
}

void update_pins_br()
{
 update_bus_br();
 pins_br->clear();
 if (fjtag.active_part < 0)
  return;
 Part *part = fjtag.parts[fjtag.active_part];
 char b[1024];
 for (int i = 0; i < (int)part->signals.size(); i++) {
  fill_signal_line(b, part->signals[i]);
  pins_br->add(b, (void *)part->signals[i]);
 }
}

void update_bus_br()
{
 update_flash_br();
 bus_br->clear();
 if (fjtag.active_bus < 0)
  return;
 bus_br->add(fjtag.buses[fjtag.active_bus]->name(), (void *) fjtag.active_bus);
}

void update_flash_br()
{
 flash_br->clear();
 if (fjtag.active_bus < 0)
  return;
 Bus *b = fjtag.buses[fjtag.active_bus];
 if (!b->flash)
  return;
 char buf[1024];
 sprintf(buf, "@bFlash device \'%s\' (cid 0x%X) detected", b->flash->name(),
  b->flash->cid());
 flash_br->add(buf);
 sprintf(buf, "@bManufacturer: \'%s\' (mid 0x%X)", b->flash->manufacturer(),
  b->flash->mid());
 flash_br->add(buf);
 sprintf(buf, "@bProtected: 0x%X", b->flash->isprotected());
 flash_br->add(buf);
 const char *info = b->flash->info();
 if (!info)
  return;
 char *t = strdup(info), *t1 = t, *t2 = t;
 while (*t) {
  if (*t == '\n') {
    *t = 0;
    flash_br->add(t1);
    t1 = t + 1;
   }
  t++;
 }
 free(t2);
}

void FJtag::update_status()
{
 char t[1024];
 sprintf(t, "Cable:  %s  at  %s  :  ", fjtag.cable_name(),
  fjtag.cable_dev_name());
 switch (fjtag.cable_status()) {
  case CABLE_NOT_PRESENT:
    strcat(t, "Not Present");
    active_part = -1;
    break;
  case CABLE_PRESENT:
    strcat(t, "Present");
    break;
  case DEVICE_ERROR:
    strcat(t, "Device error : ");
    strcat(t, fjtag.cable_dev_error());
    break;
 }
 strcat(t, "   |   Active Part : ");
 if (active_part < 0)
  strcat(t, "None");
 else {
    char b[512];
    sprintf(b, "N %d (%s)", active_part, parts[active_part]->name());
    strcat(t, b);
  }
 strcat(t, "   |   ");
 status_bar->value(t);
 status_bar->clear_visible_focus();
}
