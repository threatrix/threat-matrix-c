/*
 * Copyright (C) 2002 ETC s.r.o.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Written by Marcel Telka <marcel@telka.sk>, 2002.
 * Rewritten in c++ by Rus V. Brushkoff <rus@soyuzkt.od.ua>, 2004.
 *
 */

#ifndef	_PART_H_
#define	_PART_H_

#include <register.H>
#include <vector>
#include <string>

using namespace std;

#define DATABASE_PATH "/usr/local/share/jtag"

class instruction {
  string name_;
  tap_register *value_;
  data_register *dr_;
 public:
  instruction(const char *n, int l, const char *v) : name_(n) {
    value_ = new tap_register(l);
    value_->set(v);
    dr_ = 0;
 }
  ~instruction() { delete value_; }
  tap_register *value() { return value_; };
  data_register *&dr() { return dr_; }
  const char *name() { return name_.c_str(); }
};

#define BSBIT_UNKNOWN   0
#define BSBIT_INPUT     1
#define BSBIT_OUTPUT    2
#define BSBIT_CONTROL   3
#define BSBIT_INTERNAL  4
#define BSBIT_BIDIR     5

#define BSBIT_STATE_Z   (-1)

class bsbit;
class Part;

class signal {
    string name_;
  public:
    Part *part;
    bsbit *input, *output;
    signal(const char *n, Part *p) : name_(n) {
      input = output = 0;
      part = p;
   }
    int set(int o, int v);
    int get();
    const char *name() { return name_.c_str(); }
};

class bsbit {

  string name_;
  int bit_, type_, safe_;
  signal *sig;
  
  public:
   int control, control_value, control_state;
   bsbit(int b, const char *n, vector <signal *> &signals, int t, int s) :
    name_(n), bit_(b), type_(t), safe_(s), sig(0), control(-1) {
      vector <signal *>::iterator iter = signals.begin();
      for (; iter != signals.end(); iter++) {
        if (!strcmp((*iter)->name(), n)) {
          sig = (*iter);
          switch(type_) {
            case BSBIT_INPUT:
              sig->input = this;
              break;
            case BSBIT_OUTPUT:
              sig->output = this;
            case BSBIT_BIDIR:
              sig->input = sig->output = this;
              break;
            default:
              break;
         }
          break;
      }
    }
  }
   const char *name() { return name_.c_str(); }
   int type() { return type_; }
   void type(int t) { type_ = t; }
   int nbit() { return bit_; }
};

static inline void delete_signal(signal *x) { delete x; }
static inline void delete_bsbit(bsbit *x) { delete x; }
static inline void delete_instruction(instruction *x) { delete x; }
static inline void delete_data_register(data_register *x) { delete x; }

class Tap;

class Part {
  string manufacturer_, name_, stepping_, filename_, manufacturer_dir_,
    part_dir_;
  instruction *active_instruction_;
  vector <data_register *> data_registers;
  
 public:
  vector <signal *> signals;
  vector <bsbit *> bsbits;

  int boundary_length;
  tap_register *id;
  vector <instruction *> instructions;
  int instructions_length;
  Tap *tap;

  Part(tap_register &r, Tap *t);
  ~Part() {
    for_each(signals.begin(), signals.end(), delete_signal);
    signals.erase(signals.begin(), signals.end());
    for_each(bsbits.begin(), bsbits.end(), delete_bsbit);
    bsbits.erase(bsbits.begin(), bsbits.end());
    for_each(instructions.begin(), instructions.end(), delete_instruction);
    instructions.erase(instructions.begin(), instructions.end());
    for_each(data_registers.begin(), data_registers.end(),
      delete_data_register);
    data_registers.erase(data_registers.begin(), data_registers.end());
    delete id;
 }
  const char *manufacturer() { return manufacturer_.c_str(); }
  const char *name() { return name_.c_str(); }
  const char *stepping() { return stepping_.c_str(); }
  const char *filename() { return filename_.c_str(); }
  const char *part_dir() { return part_dir_.c_str(); }
  const char *manufacturer_dir() { return manufacturer_dir_.c_str(); }
  int find_record(string &file, tap_register &r, int bits_from,
    int bits_to, string &s1, string &s2, string &s3);
  signal *find_signal(const char *sig);
  signal *get_signal(int bit) { return signals[bit]; }
  void add_signal(const char *sig) {
    signal *s = new signal(sig, this);
    signals.push_back(s);
 }
  data_register *find_data_register(const char *dr);
  data_register *add_data_register(const char *n, int len) {
    data_register *dr = new data_register(n, len);
    data_registers.push_back(dr);
    return dr;
 }
  bsbit *&get_bit(int bit) {
    if (bit >= (int)bsbits.size())
      bsbits.resize(bit + 1);
    return bsbits[bit];
 }
  bsbit *add_bit(int b, const char *n, int t, int s) {
    bsbit *bs = new bsbit(b, n, signals, t, s);
    bsbits[b] = bs;
    return bs;
 }
  instruction *add_instruction(const char *i, int l, const char *v) {
    instruction *instr = new instruction(i, l, v);
    instructions.push_back(instr);
    return instr;
 }
  instruction *find_instruction(const char *instr);
  instruction *active_instruction() { return active_instruction_; }
  void active_instruction(instruction *instr) { active_instruction_ = instr; }
  void set_instruction(const char *i) {
    active_instruction(find_instruction(i));
 }
};

static inline void delete_part(Part *p) { delete p; }

#endif
