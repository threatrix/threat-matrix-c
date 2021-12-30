
#include <ui.H>
#include <FL/Fl_Preferences.H>
#include <cable.H>
#include <string.h>

Fl_Preferences fjtag_prefs(Fl_Preferences::USER, "7R", "FJtag");

void preferences(Fl_Menu_*, void *d)
{
 char buf[128];
 int def = !fjtag_prefs.get("Port select", buf, "Default", 100);
 if (def || !strcmp(buf, "None"))
  port_select_ch->value(0);
 else
  port_select_ch->value(buf[12] - '0' + 1);
 int cable_type;
 def = !fjtag_prefs.get("Cable type", cable_type, 0);
 if (def || !cable_type)
  cable_type_ch->value(UNKNOWN_CABLE);
 else
  cable_type_ch->value(cable_type);
 if (!d) {
  prefs_w->show();
  cable_type_ch->take_focus();
 }
}

void prefs_ok(Fl_Button*, void*)
{
 fjtag_prefs.set("Port select", port_select_ch->text());
 fjtag_prefs.set("Cable type", (int)(cable_type_ch->mvalue())->user_data());
 fjtag_prefs.flush();
 prefs_w->hide();
}

int load_prefs()
{
 preferences(0, (void *)1);
 return 0;
}
