
#include <ui.H>
#include <locale.h>
#include <stdlib.h>
#include <fjtag.H>
#include <string.h>
#include <FL/fl_ask.H>

FJtag fjtag;

int main()
{
 setlocale(LC_ALL, "");
 make_window();
 main_w->show();
 Fl::visible_focus(0);
 load_prefs();
 fjtag.setup_cable((int)(cable_type_ch->mvalue())->user_data(),
  port_select_ch->text());
 fjtag.update_status();
 cmd_i->take_focus();
 Fl::run();
 return 0;
}

void fjtag_exit()
{
 if (fl_ask("Really quit ?"))
  exit(0);
}
