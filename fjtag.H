
#ifndef _FJTAG_H_
#define _FJTAG_H_

#include <Fl_History_Input.H>
#include <FL/Fl.H>

extern void fjtag_exit();
extern int load_prefs();
extern void log(const char *fmt, ...);
extern void update_parts_br();
extern void update_pins_br();
extern void update_bus_br();
extern void update_flash_br();

#include <stdlib.h>
#define str2int(x)  strtol((x), 0, 0)

#define DETECT_PATTERN_SIZE     8
#define MAX_REGISTER_LENGTH     1024
#define TEST_COUNT              1
#define TEST_THRESHOLD          100             /* in % */

#include <state.H>
#include <register.H>
#include <part.H>
#include <cable.H>
#include <stdarg.h>
#include <string.h>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// messages
typedef void(*messagef_t)(const char *, ...);
extern messagef_t message_f;
extern void set_message_f(messagef_t mf);
extern void message(const char *fmt, ...);

#include <bus.H>

class Tap : public jtag_state {

  cable_driver *cable;
  char cable_type;
  int big_endian;

 public:
  vector <Bus *> buses;
  vector <Part *> parts;
  int active_part, active_bus;
  Tap() {
    cable_type = UNKNOWN_CABLE;
    cable = 0;
    active_bus = active_part = -1;
    big_endian = 0;
 }
  ~Tap() { if (cable) delete cable; clear_parts(); clear_buses(); }

  // tap cable
  int setup_cable(char cable_t, const char *dev);
  int cable_status() { return cable->status(); }
  const char *cable_name() { return (cable ? cable->name() : "None"); }
  const char *cable_dev_name() { return (cable ? cable->dev_name() : "None"); }
  const char *cable_dev_error() {
    return (cable ? cable->dev_error() : "No cable");
  }
  int check_cable() { return cable->check(); }
  void chain_clock(int tms, int tdi) {
    if (!cable)
      return;
    cable->clock(tms, tdi);
    clock(tms); // state clock
 }
  int chain_trst(int t) {
    int old_trst = cable->trst();
    t = cable->trst(t);
    trst(old_trst, t);
    return t;
 }
  int chain_trst() { return cable->trst(); }
  void jtag_reset() {
    chain_trst(0);
    chain_trst(1);
    reset();
 }
  void chain_shift_instructions() {
    if (!parts.size())
      return;
    for (int i = 0; i < (int)parts.size(); i++) {
      if (!(parts[i]->active_instruction())) {
        message("Part %d (%s) without active instruction", i,
          parts[i]->name());
        return;
     }
   }
    capture_ir();
    for (int i = 0; i < (int)parts.size(); i++)
      shift_register(*((parts[i]->active_instruction())->value()),
        ((i + 1) == (int)parts.size()));
 }
  void chain_shift_data_registers(int capture_output) {
    if (!parts.size())
      return;
    for (int i = 0; i < (int)parts.size(); i++) {
      if (!(parts[i]->active_instruction())) {
        message("Part %d (%s) without active instruction", i,
          parts[i]->name());
        return;
     }
      if (!((parts[i]->active_instruction())->dr())) {
        message("Part %d (%s) without data register", i, parts[i]->name());
        return;
     }
   }
    capture_dr();
    for (int i = 0; i < (int)parts.size(); i++)
      if (capture_output)
        shift_register(*((parts[i]->active_instruction())->dr()->in),
          *((parts[i]->active_instruction())->dr()->out),
          ((i + 1) == (int)parts.size()));
      else
        shift_register(*((parts[i]->active_instruction())->dr()->in),
          ((i + 1) == (int)parts.size()));
 }
  // tap machine
  void reset() {
    state(Test_Logic_Reset);
    chain_clock(1, 0);
    chain_clock(1, 0);
    chain_clock(1, 0);
    chain_clock(1, 0);
    // Test-Logic-Reset
    chain_clock(1, 0);
    // Run-Test/Idle
    chain_clock(0, 0);    
 }
  void shift_register(tap_register &in, int exit_) {
    tap_register out(0);
    shift_register(in, out, exit_);
 }
  void shift_register(tap_register &in, tap_register &out, int exit_) {
    if (!(state() & TAPSTAT_SHIFT))
      message("shift_register: Invalid state: %2X", state());
    // Capture-DR, Capture-IR, Shift-DR, Shift-IR, Exit2-DR or Exit2-IR state
    if (state() & TAPSTAT_CAPTURE)
      chain_clock(0, 0); // save last TDO bit :-)
    for (int i = 0; i < in.len(); i++) {
      if (out.len() && (i < out.len()))
        out[i] = cable->tdo();
      // Shift (& Exit1)
      chain_clock((exit_ && ((i + 1) == in.len())) ? 1 : 0, in[i]);
   }
    // Shift-DR, Shift-IR, Exit1-DR or Exit1-IR state
    if (exit_) {
      chain_clock(1, 0); // Update-DR or Update-IR
      chain_clock(0, 0); // Run-Test/Idle
   }
 }
  void capture_ir() {
    if ((state() & (TAPSTAT_RESET | TAPSTAT_IDLE)) != TAPSTAT_IDLE)
      message("capture_ir: Invalid state: %2X", state());
    // Run-Test/Idle or Update-DR or Update-IR state
    chain_clock(1, 0); // Select-DR-Scan
    chain_clock(1, 0); // Select-IR-Scan
    chain_clock(0, 0); // Capture-IR    
 }
  void capture_dr() {
    if ((state() & (TAPSTAT_RESET | TAPSTAT_IDLE)) != TAPSTAT_IDLE)
      message("capture_dr: Invalid state: %2X", state());
    // Run-Test/Idle or Update-DR or Update-IR state
    chain_clock(1, 0); // Select-DR-Scan
    chain_clock(0, 0); // Capture-DR
 }
  int detect_parts();
  int detect_register_size();
  void discovery();
  void clear_parts() {
    for_each(parts.begin(), parts.end(), delete_part);
    parts.erase(parts.begin(), parts.end());
  }
  void clear_buses() {
    for_each(buses.begin(), buses.end(), delete_bus);
    buses.erase(buses.begin(), buses.end());
  }
  void add_part(Part *p) { parts.push_back(p); }
  signal *find_signal(const char *s) {
    if (!parts.size())
      return 0;
    signal *sig;
    for (int i = 0; i < (int)parts.size(); i++)
      if ((sig = parts[i]->find_signal(s)))
        return sig;
    return 0;
  }
  Bus *add_bus(const char *b);
  void set_instruction(const char *i) {
    vector <Part *>::iterator iter = parts.begin();
     for (; iter != parts.end(); iter++)
      (*iter)->set_instruction(i);
 }
  int set_signal(Part *p, signal *s, int o, int v);
  int get_signal(Part *p, signal *s);

  // commands
  int parse_definition_file(Part *p);
  int parse_definition_file(const char *fname);
  int run(string &cmd_line);
  int run(const char *l) { string t(l); return run(t); }
  int cmd_frequency(vector <string> &args);
  int cmd_discovery(vector <string> &args);
  int cmd_detect(vector <string> &args);
  int cmd_signal(vector <string> &args);
  int cmd_bit(vector <string> &args);
  int cmd_register(vector <string> &args);
  int cmd_initbus(vector <string> &args);
  int cmd_print(vector <string> &args);
  int cmd_part(vector <string> &args);
  int cmd_bus(vector <string> &args);
  int cmd_instruction(vector <string> &args);
  int cmd_shift(vector <string> &args);
  int cmd_dr(vector <string> &args);
  int cmd_get(vector <string> &args);
  int cmd_set(vector <string> &args);
  int cmd_endian(vector <string> &args);
  int cmd_peek(vector <string> &args);
  int cmd_poke(vector <string> &args);
  int cmd_flashdetect(vector <string> &args);
  int cmd_flashread(vector <string> &args);
  int cmd_flashburn(vector <string> &args);
  int cmd_flasherase(vector <string> &args);
  int cmd_flashprotect(vector <string> &args);
  int cmd_script(vector <string> &args);
  int cmd_include(vector <string> &args);
  int cmd_help(vector <string> &args);
  
 public:
};

#define CMD_UNKNOWN       0
#define CMD_FREQUENCY     1
#define CMD_DISCOVERY     2
#define CMD_DETECT        3
#define CMD_SIGNAL        4
#define CMD_BIT           5
#define CMD_REGISTER      6
#define CMD_INITBUS       7
#define CMD_PRINT         8
#define CMD_PART          9
#define CMD_BUS           10
#define CMD_INSTRUCTION   11
#define CMD_SHIFT         12
#define CMD_DR            13
#define CMD_GET           14
#define CMD_SET           15
#define CMD_ENDIAN        16
#define CMD_PEEK          17
#define CMD_POKE          18
#define CMD_FLASHDETECT   19
#define CMD_FLASHREAD     20
#define CMD_FLASHBURN     21
#define CMD_FLASHERASE    22
#define CMD_FLASHPROTECT  23
#define CMD_SCRIPT        24
#define CMD_INCLUDE       25
#define CMD_HELP          26

class FJtag : public Tap {

 public:

  FJtag() { set_message_f(log); }
  ~FJtag() { }
  void update_status();
};

extern FJtag fjtag;

#endif
