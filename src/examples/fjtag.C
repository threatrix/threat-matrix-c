
#include <ui.H>
#include <fjtag.H>
#include <stdarg.h>
#include <string.h>
#include <FL/fl_ask.H>

#define MAX_MESSAGE_SIZE  80

messagef_t message_f;

void log(const char *fmt, ...)
{
 char buffer[2048]; // длина "раскрытого" сообщения должна быть не > 2048 b
 va_list list;
 va_start(list, fmt);
 vsprintf(buffer, fmt, list);
 va_end(list);
 if (strlen(buffer) > MAX_MESSAGE_SIZE) {
  char t = buffer[MAX_MESSAGE_SIZE];
  buffer[MAX_MESSAGE_SIZE] = 0;
  log(buffer);
  buffer[MAX_MESSAGE_SIZE] = t;
  log(buffer + MAX_MESSAGE_SIZE);
  return;
 }
 log_br->add(buffer);
 log_br->topline(log_br->size() + 1);
 Fl::wait(.5);
}

void set_message_f(messagef_t mf)
{
 message_f = mf;
}

void message(const char *fmt, ...)
{
 char buffer[2048]; // длина "раскрытого" сообщения должна быть не > 2048 b
 va_list list;
 va_start(list, fmt);
 vsprintf(buffer, fmt, list);
 va_end(list);
 log(buffer);
}

void open_flash_image(Fl_Menu_*, void*)
{
}

void save_flash_image(Fl_Menu_*, void*)
{
}

void autodetect_flash(Fl_Menu_*, void*)
{
 log("Detecting flash ...");
 fjtag.run("flashdetect");
 update_flash_br();
}

void autodetect_chain(Fl_Menu_*, void*)
{
 if (fjtag.check_cable() != CABLE_PRESENT) {
  fjtag.update_status();
  fl_message("No cable present !");
  return;
 }
 log("Detecting parts ...");
 fjtag.detect_parts();
 fjtag.update_status();
 update_parts_br();
}

void part_br_cb(Fl_Browser *br, void *)
{
 void *d = br->data(br->value());
 br->deselect();
 if (d == (void *)-1)
  return;
 for (int i = 0; i < br->size(); i++)
  if (br->data(i + 1) == d)
    br->value(i + 1);
 fjtag.active_part = (int) d;
 update_pins_br();
}

extern void fill_signal_line(char *, signal *);

void pins_br_cb(Fl_Browser *br, void *)
{
 signal *s = (signal *) br->data(br->value());
 s->part->set_instruction("EXTEST");
 fjtag.chain_shift_instructions();
 fjtag.chain_shift_data_registers(1);
 char b[1024];
 fill_signal_line(b, s);
 br->text(br->value(), b); 
}
