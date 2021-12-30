
#include <fjtag.H>

int Tap::setup_cable(char cable_t, char const *dev)
{
 if (cable)
  delete cable;
 cable = 0;
 switch(cable_t) {
  case ALTERA_BYTEBLASTER:
    cable = new altera_cable_driver(dev);
    break;
  case UNKNOWN_CABLE:
    cable = new default_cable_driver(dev);
  default:
    return -1;
 }
 return 0;
}

int bus2num(const char *bus)
{
 if (!strcmp(bus, "e1c_maxloader"))
  return BUS_E1C_MAXLOADER;
 if (!strcmp(bus, "nios"))
  return BUS_NIOS;
 return BUS_UNKNOWN;
}

Bus *Tap::add_bus(const char *b)
{
 Bus *bus;
 switch(bus2num(b)) {
  case BUS_E1C_MAXLOADER:
    bus = new e1c_maxloader_bus(this);
    break;
  case BUS_NIOS:
    bus = new nios_bus(this);
    break;
  case BUS_UNKNOWN:
  default:
    message("unknown bus %s", b);
    return 0;
 }
 buses.push_back(bus);
 active_bus = buses.size() - 1;
 return bus;
}

int Tap::detect_parts()
{
 clear_buses();
 clear_parts();
 // Detect IR length
 reset();
 capture_ir();
 int irlen = detect_register_size();
 if (irlen < 1)
  return 0;
 message("IR length: %d", irlen);
 // Allocate IR
 tap_register *ir = new tap_register(irlen, 1);
 shift_register(*ir, 1);
 delete ir;
 // Detect chain length
 capture_dr();
 int chlen = detect_register_size();
 if (chlen < 1) {
  message("Unable to detect JTAG chain length !");
  return 0;
 }
 message("Chain length: %d", chlen);
 // Allocate registers and parts
 tap_register *one = new tap_register(1, 1);
 tap_register *ones = new tap_register(31, 1);
 tap_register *br = new tap_register(1);
 tap_register *id = new tap_register(32);
 // Detect parts
 reset();
 capture_dr();
 for (int i = 0; i < chlen; i++) {
  tap_register *did = br;
  Part *part;
  
  shift_register(*one, *br, 0);
  if (!one->cmp(*br)) {
    // part with id
    shift_register(*ones, *id, 0);
    id->shift_left(1);
    (*id)[0] = 1;
    did = id;
  }
  message("Device Id: %s", (did->strval())->c_str());
  part = new Part(*did, this);
  add_part(part);
  message("  Manufacturer: %s", part->manufacturer());
  message("  Part N %d: %s", parts.size() - 1, part->name());
  message("  Stepping: %s", part->stepping());
  message("  Definition filename: %s", part->filename());
  active_part = parts.size() - 1;
  parse_definition_file(part);
  if (!part->active_instruction())
    part->active_instruction(part->find_instruction("IDCODE"));
 }
  for (int i = 0; i < 32; i++) {
    shift_register(*one, *br, 0);
    if (one->cmp(*br)) {
      message("Error: Unable to detect JTAG chain end !");
      break;
  }
 }
  shift_register(*one, 1);
  delete one;
  delete ones;
  delete br;
  delete id;
  if (!parts.size())
    return 0;
  set_instruction("SAMPLE/PRELOAD");
  chain_shift_instructions();
  chain_shift_data_registers(1);
  set_instruction("BYPASS");
  chain_shift_instructions();
  return parts.size();
}

int Tap::detect_register_size()
{
 tap_register *rz, *rout, *rpat;
 int len;
   for (len = 1; len <= MAX_REGISTER_LENGTH; len++) {
    int p, ok = 0;
    rz = new tap_register(len);
    rout = new tap_register(DETECT_PATTERN_SIZE + len);
    rpat = new tap_register(DETECT_PATTERN_SIZE + len);
    rpat->inc();
    for (p = 1; p < (1 << DETECT_PATTERN_SIZE); p++) {
//      const char *s;
      ok = 0;
      
//      s = rpat->strval()->c_str()
//      while (*s)
//        s++;
      for (int i = 0; i < TEST_COUNT; i++) {
        shift_register(*rz, 0);
        shift_register(*rpat, *rout, 0);
        rout->shift_right(len);
        if (!rpat->cmp(*rout))
          ok++;
      }
      if ((100 * ok / TEST_COUNT) < TEST_THRESHOLD) {
        ok = 0;
        break;
      }
      rpat->inc();
    }
      delete rz;
      delete rout;
      delete rpat;
      if (ok)
        return len;
  }
    return -1;
}

void Tap::discovery()
{
 message("Discovering new parts ...");
 // detecting IR size
 jtag_reset();
 message("Detecting IR length ... ");
 capture_ir();
 int irlen = detect_register_size();
 if (irlen < 1) {
  message("Invalid IR length %d !", irlen);
  return;
 }
 message("IR length = %d", irlen);
 // all 1 is BYPASS in all parts, so DR length gives number of parts
 tap_register *ir = new tap_register(irlen, 1);
 tap_register *irz = ir->dup();
 while (1) {
  jtag_reset();
  capture_ir();
  shift_register(*ir, 1);
  capture_dr();
  int rs = detect_register_size();
  message("Detecting DR length for IR %s ... %d", (ir->strval())->c_str(), rs);
  ir->inc();
  if (!ir->cmp(*irz))
    break;
 }
  delete ir;
  delete irz;
}

#include <fstream>
#include <ctype.h>
#include <iostream>

int cmd2int(const char *cmd)
{
 if (!strcmp(cmd, "frequency"))
   return CMD_FREQUENCY;
 if (!strcmp(cmd, "discovery"))
   return CMD_DISCOVERY;
 if (!strcmp(cmd, "detect"))
   return CMD_DETECT;
 if (!strcmp(cmd, "signal"))
   return CMD_SIGNAL;
 if (!strcmp(cmd, "bit"))
   return CMD_BIT;
 if (!strcmp(cmd, "register"))
   return CMD_REGISTER;
 if (!strcmp(cmd, "initbus"))
   return CMD_INITBUS;
 if (!strcmp(cmd, "print"))
   return CMD_PRINT;
 if (!strcmp(cmd, "part"))
   return CMD_PART;
 if (!strcmp(cmd, "bus"))
   return CMD_BUS;
 if (!strcmp(cmd, "instruction"))
   return CMD_INSTRUCTION;
 if (!strcmp(cmd, "shift"))
   return CMD_SHIFT;
 if (!strcmp(cmd, "dr"))
   return CMD_DR;
 if (!strcmp(cmd, "get"))
   return CMD_GET;
 if (!strcmp(cmd, "set"))
   return CMD_SET;
 if (!strcmp(cmd, "endian"))
   return CMD_ENDIAN;
 if (!strcmp(cmd, "peek"))
   return CMD_PEEK;
 if (!strcmp(cmd, "poke"))
   return CMD_POKE;
 if (!strcmp(cmd, "flashdetect"))
   return CMD_FLASHDETECT;
 if (!strcmp(cmd, "flashread"))
   return CMD_FLASHREAD;
 if (!strcmp(cmd, "flashburn"))
   return CMD_FLASHBURN;
 if (!strcmp(cmd, "flasherase"))
   return CMD_FLASHERASE;
 if (!strcmp(cmd, "flashprotect"))
   return CMD_FLASHPROTECT;
 if (!strcmp(cmd, "script"))
   return CMD_SCRIPT;
 if (!strcmp(cmd, "include"))
   return CMD_INCLUDE;
 if (!strcmp(cmd, "help"))
   return CMD_HELP;
 return CMD_UNKNOWN;
}

int Tap::cmd_frequency(vector <string> &args)
{
 if (args.size() == 1) // print
  message("Current TCK frequency is %u Hz", cable->frequency());
 if (args.size() != 2)
  return -1;
 message("Setting TCK frequency to %u Hz", str2int(args[1].c_str()));
 cable->frequency(str2int(args[1].c_str()));
 return 0;
}

int Tap::cmd_discovery(vector <string> &args)
{
 if (args.size() != 1)
  return -1;
 if (cable->status() != CABLE_PRESENT) {
  message("cmd_discovery: No cable present");
  return -2;
 }
 discovery();
 return 0;
}

int Tap::cmd_detect(vector <string> &args)
{
 if (args.size() != 1)
  return -1;
 if (cable->status() != CABLE_PRESENT) {
  message("cmd_detect: No cable present");
  return -2;
 }
 detect_parts();
 return 0;
}

int Tap::cmd_signal(vector <string> &args)
{
 if (args.size() != 2) {
  message("cmd_signal: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (cable->status() != CABLE_PRESENT) {
  message("cmd_signal: No cable present");
  return -2;
 }
 if (!parts.size()) {
  message("cmd_signal: Run \"detect\" first.");
  return -3;
 }
 if ((active_part < 0) || (active_part >= (int)parts.size())) {
  message("cmd_signal: No active part");
  return -4;
 }
 if (parts[active_part]->find_signal(args[1].c_str())) {
  message("cmd_signal: Signal %s already defined", args[1].c_str());
  return -5;
 }
 parts[active_part]->add_signal(args[1].c_str());
 return 0;
}

int Tap::cmd_bit(vector <string> &args)
{
 if ((args.size() != 5) && (args.size() != 8)) {
  message("cmd_bit: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (cable->status() != CABLE_PRESENT) {
  message("cmd_bit: No cable present");
  return -2;
 }
 if (!parts.size()) {
  message("cmd_bit: Run \"detect\" first.");
  return -3;
 }
 if ((active_part < 0) || (active_part >= (int)parts.size())) {
  message("cmd_bit: No active part");
  return -4;
 }
 data_register *bsr = parts[active_part]->find_data_register("BSR");
 if (!bsr) {
  message("Missing Boundary Scan Register (BSR)");
  return 1;
 }
 // bit number
 int bit = str2int(args[1].c_str());
 if (bit >= bsr->in->len()) {
  message("Invalid boundary bit number: %d", bit);
  return 1;
 }
 if (parts[active_part]->get_bit(bit)) {
  message("duplicate bit N %d declaration", bit);
  return 1;
 }
 // bit type
 if (args[2].size() != 1)
  return -5;
 int type;
 switch((args[2].c_str())[0]) {
    case 'I':
      type = BSBIT_INPUT;
      break;
    case 'O':
      type = BSBIT_OUTPUT;
      break;
    case 'B':
      type = BSBIT_BIDIR;
      break;
    case 'C':
      type = BSBIT_CONTROL;
      break;
    case 'X':
      type = BSBIT_INTERNAL;
      break;
    default:
      return -6;      
 }
  // default (safe) value
  if (args[3].size() != 1)
   return -7;
  int safe = ((args[3].c_str())[0] == '1') ? 1 : 0;
  bsr->in->operator[](bit) = safe;
  parts[active_part]->add_bit(bit, args[4].c_str(), type, safe);
  if (args.size() == 5)
    return 0;
  int control = str2int(args[5].c_str());
  if (control >= bsr->in->len()) {
    message("invalid control bit number %d", control);
    return 0;
  }
  parts[active_part]->get_bit(bit)->control = control;
  // control value
  if (args[6].size() != 1)
    return -8;
  parts[active_part]->get_bit(bit)->control_value =
    ((args[6].c_str())[0] == '1') ? 1 : 0;
  // control state
  if ((args[7].size() != 1) || ((args[7].c_str())[0] != 'Z'))
    return -9;
  parts[active_part]->get_bit(bit)->control_state = BSBIT_STATE_Z;
  return 0;
}

int Tap::cmd_register(vector <string> &args)
{
 if (args.size() != 3) {
  message("cmd_register: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (cable->status() != CABLE_PRESENT) {
  message("cmd_register: No cable present");
  return -2;
 }
 if (!parts.size()) {
  message("cmd_register: Run \"detect\" first.");
  return -3;
 }
 if ((active_part < 0) || (active_part >= (int)parts.size())) {
  message("cmd_register: No active part");
  return -4;
 }
 if (parts[active_part]->find_data_register(args[1].c_str())) {
  message("Data register '%s' already defined", args[1].c_str());
  return -5;
 }
 data_register *dr = parts[active_part]->add_data_register(args[1].c_str(),
  str2int(args[2].c_str()));
 // Boundary Scan Register
 if (!strcmp(args[1].c_str(), "BSR")) {
  parts[active_part]->boundary_length = str2int(args[2].c_str());
  for (int i = 0; i < parts[active_part]->boundary_length; i++)
    parts[active_part]->get_bit(i) = 0;
 }
 // Device Identification Register
 if (!strcmp(args[1].c_str(), "DIR"))
  dr->out->set((parts[active_part]->id)->strval()->c_str());
 return 0;
}

int Tap::cmd_initbus(vector <string> &args)
{
 if (args.size() != 2) {
  message("cmd_initbus: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (cable->status() != CABLE_PRESENT) {
  message("cmd_initbus: No cable present");
  return -2;
 }
 if (!parts.size()) {
  message("cmd_initbus: Run \"detect\" first.");
  return -3;
 }
 if ((active_part < 0) || (active_part >= (int)parts.size())) {
  message("cmd_initbus: No active part");
  return -4;
 }
 return add_bus(args[1].c_str()) ? 0 : -5;
}

int Tap::cmd_print(vector <string> &args)
{
 message("cmd_print: not implemented");
 return -1;
}

int Tap::cmd_part(vector <string> &args)
{
 if (args.size() == 1) { // show current part
  if (active_part >= 0)
    message("current part %d (%s)", active_part,
      parts[active_part]->name());
  else
    message("no active part was set");
  return 0;
 }
 if (args.size() != 2) {
  message("cmd_part: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (cable->status() != CABLE_PRESENT) {
  message("cmd_part: No cable present");
  return -2;
 }
 if (!parts.size()) {
  message("cmd_part: Run \"detect\" first.");
  return -3;
 }
 int part = str2int(args[1].c_str());
 if (part >= (int)parts.size()) {
  message("cmd_part: Invalid part number %d", part);
  return -4;
 }
 active_part = part;
 message("Current part is %d (%s)", active_part, parts[active_part]->name());
 return 0;
}

int Tap::cmd_bus(vector <string> &args)
{
 if (args.size() == 1) { // show current bus
  if (active_bus < 0)
    message("No active bus was set");
  else
    message("Current bus is \'%s\'", buses[active_bus]->name());
  return 0;
 }
 if (args.size() != 2) {
  message("cmd_bus: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (cable->status() != CABLE_PRESENT) {
  message("cmd_bus: No cable present");
  return -2;
 }
 if (!parts.size()) {
  message("cmd_bus: Run \"detect\" first.");
  return -3;
 }
 int bus = str2int(args[1].c_str());
 if (bus >= (int)buses.size()) {
  message("cmd_bus: Invalid bus number %d", bus);
  return -4;
 }
 active_bus = bus;
 return 0;
}

int Tap::cmd_instruction(vector <string> &args)
{
 if (cable->status() != CABLE_PRESENT) {
  message("cmd_instruction: No cable present");
  return -1;
 }
 if (!parts.size()) {
  message("cmd_instruction: Run \"detect\" first.");
  return -2;
 }
 if ((active_part < 0) || (active_part >= (int)parts.size())) {
  message("cmd_instruction: No active part");
  return -3;
 }
 Part *part = parts[active_part];
 if (args.size() == 1) { // show instructions
  if (!part->instructions.size()) {
    message("No instructions defined for part %d", active_part);
    return 0;
  }
  for (int i = 0; i < (int)part->instructions.size(); i++)
    message("Instruction %s, register %s", part->instructions[i]->name(),
      part->instructions[i]->dr()->name());
  return 0;
 }
 if (args.size() == 2) {
  part->set_instruction(args[1].c_str());
  if (!part->active_instruction()) {
    message("unknown instruction \'%s\'", args[1].c_str());
    return 0;
  }
 }
 if (args.size() == 3) {
  if (strcmp(args[1].c_str(), "length"))
    return -1;
  if (part->instructions.size()) {
    message("instruction length is already set and used");
    return 0;
  }
  part->instructions_length = str2int(args[2].c_str());
  return 0;
 }
 if (args.size() == 4) {
  if ((int)args[2].size() != part->instructions_length) {
    message("invalid instruction length - %d", args[2].size());
    return 0;
  }
  if (part->find_instruction(args[1].c_str())) {
    message("Instruction \'%s\' already defined", args[1].c_str());
    return 0;
  }
  instruction *i = part->add_instruction(args[1].c_str(),
    part->instructions_length, args[2].c_str());
  i->dr() = part->find_data_register(args[3].c_str());
  if (!(i->dr())) {
    message("unknown data register \'%s\'", args[3].c_str());
    return 0;
  }
    return 0;
 }
 return -4;
}

int Tap::cmd_shift(vector <string> &args)
{
 if (args.size() != 2) {
  message("cmd_shift: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (cable->status() != CABLE_PRESENT) {
  message("cmd_shift: No cable present");
  return -2;
 }
 if (!strcmp(args[1].c_str(), "ir")) {
  chain_shift_instructions();
  return 0;
 }
 if (!strcmp(args[1].c_str(), "dr")) {
  chain_shift_data_registers(1);
  return 0;
 }
 return -1;
}

int Tap::cmd_dr(vector <string> &args)
{
 if ((args.size() < 1) || (args.size() > 2)) {
  message("cmd_dr: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (cable->status() != CABLE_PRESENT) {
  message("cmd_dr: No cable present");
  return -2;
 }
 if (!parts.size()) {
  message("cmd_dr: Run \"detect\" first.");
  return -3;
 }
 if ((active_part < 0) || (active_part >= (int)parts.size())) {
  message("cmd_dr: No active part");
  return -4;
 }
 int dir = 1;
 if (args.size() == 2) {
  if (!strcmp(args[1].c_str(), "in"))
    dir = 0;
  else if (!strcmp(args[1].c_str(), "out"))
    dir = 1;
  else
    return -1;
 }
 if (!parts[active_part]->active_instruction()) {
  message("part %d (%s) without active instruction",
    active_part, parts[active_part]->name());
  return 0;
 }
 if (!parts[active_part]->active_instruction()->dr()) {
  message("part %d (%s) without active data register",
    active_part, parts[active_part]->name());
  return 0;
 }
 tap_register *r;
 if (dir)
  r = parts[active_part]->active_instruction()->dr()->out;
 else
  r = parts[active_part]->active_instruction()->dr()->in;
 message("%s", r->strval()->c_str());
 return 0;
}

int Tap::cmd_get(vector <string> &args)
{
 if (args.size() != 3) {
  message("cmd_get: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (cable->status() != CABLE_PRESENT) {
  message("cmd_get: No cable present");
  return -2;
 }
 if (!parts.size()) {
  message("cmd_get: Run \"detect\" first.");
  return -3;
 }
 if ((active_part < 0) || (active_part >= (int)parts.size())) {
  message("cmd_get: No active part");
  return -4;
 }
 if (strcmp(args[1].c_str(), "signal"))
  return -5;
 signal *s = parts[active_part]->find_signal(args[2].c_str());
 if (!s) {
  message("signal \'%s\' not found", args[2].c_str());
  return 0;
 }
 int data = get_signal(parts[active_part], s);
 if (data >= 0)
  message("%s = %d", args[2].c_str(), data);
 return 0;
}

int Tap::cmd_set(vector <string> &args)
{
 if ((args.size() < 4) || (args.size() > 5)) {
  message("cmd_set: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (cable->status() != CABLE_PRESENT) {
  message("cmd_set: No cable present");
  return -2;
 }
 if (!parts.size()) {
  message("cmd_set: Run \"detect\" first.");
  return -3;
 }
 if ((active_part < 0) || (active_part >= (int)parts.size())) {
  message("cmd_set: No active part");
  return -4;
 }
 if (strcmp(args[1].c_str(), "signal"))
  return -5;
 // direction
 if (strcmp(args[3].c_str(), "in") && strcmp(args[3].c_str(), "out"))
  return -6;
 int dir = (!strcmp(args[3].c_str(), "in") ? 0 : 1);
 int data = 0;
 if (dir) {
  if (args.size() == 4)
    data = 0;
  else
    data = str2int(args[4].c_str());
  if (data > 1)
    return -7;
 }
 signal *s = parts[active_part]->find_signal(args[2].c_str());
 if (!s) {
  message("signal \'%s\' not found", args[2].c_str());
  return 0;
 }
 set_signal(parts[active_part], s, dir, data);
 return 0;
}

int Tap::cmd_endian(vector <string> &args)
{
 if (args.size() > 2) {
  message("cmd_endian: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (args.size() == 1) {
  if (big_endian)
    message("Endianess for external files: %s", big_endian ? "big" : "little");
    return 0;
 }
 if (!strcmp(args[1].c_str(), "little")) {
  big_endian = 0;
  return 0;
 } else if (!strcmp(args[1].c_str(), "big")) {
    big_endian = 1;
    return 0;
 }
 return -1;
}

int Tap::cmd_peek(vector <string> &args)
{
 if (args.size() != 2) {
  message("cmd_peek: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (active_bus < 0) {
  message("No active bus");
  return 0;
 }
 buses[active_bus]->prepare();
 uint32_t addr = str2int(args[1].c_str());
 message("bus_read from 0x%08X = 0x%X", addr,
  buses[active_bus]->read(addr) & ((1 << buses[active_bus]->width()) - 1));
 return 0;
}

int Tap::cmd_poke(vector <string> &args)
{
 if (args.size() != 3) {
  message("cmd_peek: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (active_bus < 0) {
  message("No active bus");
  return 0;
 }
 buses[active_bus]->prepare();
 uint32_t addr = str2int(args[1].c_str());
 uint32_t val = str2int(args[2].c_str());
 buses[active_bus]->write(addr, val); 
 return 0;
}

int Tap::cmd_flashdetect(vector <string> &args)
{
 if (args.size() != 1) {
  message("cmd_flashdetect: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (active_bus < 0) {
  message("No active bus");
  return 0;
 }
 if (!cfi_detect(buses[active_bus])) {
  if (!jedec_detect(buses[active_bus])) {
    message("Flash not found");
    return 0;
  }
 }
 buses[active_bus]->print_flash_info();
 message("Flash device \'%s\' (cid 0x%X) detected",
  buses[active_bus]->flash->name(), buses[active_bus]->flash->cid());
 message(" Manufacturer: \'%s\' (mid 0x%X)",
  buses[active_bus]->flash->manufacturer(), buses[active_bus]->flash->mid());
 message(" Protected: 0x%X", buses[active_bus]->flash->isprotected());
 return 0;
}

// flashread 0xoffset 0xlen file
// flashread 0xoffset file
// flashread file
int Tap::cmd_flashread(vector <string> &args)
{
 if ((args.size() != 2) && (args.size() != 3) && (args.size() != 4)) {
  message("cmd_flashread: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (active_bus < 0) {
  message("No active bus");
  return 0;
 }
 int offset = 0, len = 0;
 string fname;
 if (args.size() == 2) {
  offset = 0;
  len = buses[active_bus]->flash->size();
  fname = args[1];
 } else if (args.size() == 3) {
  offset = str2int(args[1].c_str());
  len = buses[active_bus]->flash->size();
  fname = args[2];
 } else if (args.size() == 4) {
  offset = str2int(args[1].c_str());
  len = str2int(args[2].c_str());
  fname = args[3];
 }
 ofstream of(fname.c_str(), ios::out | ios::binary | ios::trunc);
 if (!of) {
  message("Can't open file %s", fname.c_str());
  return -2;
 }
 uint32_t tmp;
 message("Reading from %d (0x%X), size %d (0x%X) ...", offset, offset, len,
  len);
 for (int i = 0; i < len; i++) {
  tmp = buses[active_bus]->flash->read(offset + i);
  of.write((const char *)&tmp,
    (buses[active_bus]->width() / 8)); // not portable ;(
  if (i && !(i % (1024 * 10)))
    message("%d Kbytes read", ((i + 1) / 1024));
 }
 of.close();
 message("Done");
 return 0;
}

// flashburn file
// flashburn file filetype 0xoffset 0xlen
// flashburn file filetype 0xoffset
// flashburn 0xoffset 0xdata
//
int Tap::cmd_flashburn(vector <string> &args)
{
 if ((args.size() != 2) && (args.size() != 3) && (args.size() != 4)) {
  message("cmd_flashburn: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (active_bus < 0) {
  message("No active bus");
  return 0;
 }
 int flash_len =
  buses[active_bus]->flash->size() * (buses[active_bus]->width() / 8),
  file_len = 0, offset = 0;
 string fname;
 if (args.size() == 5) {
  message("Sorry, only binary file type is supported.");
  offset = str2int(args[3].c_str());
  fname = args[1];
  file_len = str2int(args[4].c_str()) * (buses[active_bus]->width() / 8);
 } else if (args.size() == 4) {
  message("Sorry, only binary file type is supported.");
  offset = str2int(args[3].c_str());
  fname = args[1];
 } else if (args.size() == 3) { // burn offset data
  return buses[active_bus]->flash->burn(str2int(args[1].c_str()),
    str2int(args[2].c_str()));
 } else if (args.size() == 2) {
  fname = args[1];
 }
 ifstream fi(fname.c_str(), ios::in | ios::binary);
 if (!fi) {
  message("Can't open file %s", fname.c_str());
  return -2;
 }
 if (!file_len) {
   fi.seekg(0, ios::end);
   file_len = fi.tellg() / (buses[active_bus]->width() / 8);
   if (fi.tellg() % (buses[active_bus]->width() / 8))
    message("File size (%d) is not buswidth aware - last bytes will be "
      "not written", (int)fi.tellg());
   fi.seekg(0, ios::beg);
 }
 if (file_len > flash_len) {
  message("Too big file array %d (flash size %d)", file_len, flash_len);
  fi.close();
  return -3;
 }
 char buf[file_len * (buses[active_bus]->width() / 8)];
 fi.read(buf, file_len * (buses[active_bus]->width() / 8));
 if (!fi.good() || fi.eof()) {
  message("Error reading %s", fname.c_str());
  fi.close();
  return -4;
 }
 message("Burning %s at %d (0x%X), size %d (0x%X) ...", fname.c_str(),
  offset, offset, file_len, file_len);
 int res = buses[active_bus]->flash->burn(offset, (unsigned char *)buf,
  file_len);
 fi.close();
 message("Done");
 return res;
}

int Tap::cmd_flasherase(vector <string> &args)
{
 if ((args.size() < 1) && (args.size() > 2)) {
  message("cmd_flasherase: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (active_bus < 0) {
  message("No active bus");
  return 0;
 }
 if (!buses[active_bus]->flash) {
   message("No active flash");
   return 0;
 }
 if (args.size() == 1)
   buses[active_bus]->flash->chip_erase();
 else
   buses[active_bus]->flash->sector_erase(str2int(args[1].c_str()));
 return 0;
}

// flashprotect 0xsector set/clear/show
//
int Tap::cmd_flashprotect(vector <string> &args)
{
 if (args.size() != 3) {
  message("cmd_flashprotect: Invalid number of args (%d)", args.size());
  return -1;
 }
 if (active_bus < 0) {
  message("No active bus");
  return 0;
 }
 if (!buses[active_bus]->flash) {
   message("No active flash");
   return 0;
 }
 if (!strcmp(args[2].c_str(), "show")) {
  int sector = str2int(args[1].c_str());
  int ip = buses[active_bus]->flash->isprotected(sector);
  if (ip < 0)
    return -2;
  message("Sector %d (0x%X) %s protected", sector, sector, ip ? "is" : "not");
 } else if (!strcmp(args[2].c_str(), "set")) {
 } else if (!strcmp(args[2].c_str(), "clear")) {
 } else
    return -3;
 return 0;
}

int Tap::cmd_script(vector <string> &args)
{
 if (args.size() != 2) {
  message("cmd_script: Invalid number of args (%d)", args.size());
  return -1;
 }
 return parse_definition_file(args[1].c_str());
}

int Tap::cmd_include(vector <string> &args)
{
 if (args.size() != 2) {
  message("cmd_include: Invalid number of args (%d)", args.size());
  return -1;
 }
 string fname = (string)DATABASE_PATH + (string)"/" + args[1];
 return parse_definition_file(fname.c_str());
}

int Tap::cmd_help(vector <string> &args)
{
 message("need to be some help ...");
 return 0;
}

void brake_to_args(string &cmd_line, vector <string> &args)
{
 string spaces = " \t\r\n\f\v";
 string::size_type pos = 0, prev_pos = 0;
 while (1) {
  pos = cmd_line.find_first_of(spaces, pos);
  args.push_back(cmd_line.substr(prev_pos, pos - prev_pos));
  if (pos == string::npos)
    break;
  prev_pos = ++pos;
 }
}

int Tap::run(string &cmd_line)
{
 vector <string> args;
 brake_to_args(cmd_line, args);
 switch (cmd2int(args[0].c_str())) {
  case CMD_FREQUENCY:
    return cmd_frequency(args);
  case CMD_DISCOVERY:
    return cmd_discovery(args);
  case CMD_DETECT:
    return cmd_detect(args);
  case CMD_SIGNAL:
    return cmd_signal(args);
  case CMD_BIT:
    return cmd_bit(args);
  case CMD_REGISTER:
    return cmd_register(args);
  case CMD_INITBUS:
    return cmd_initbus(args);
  case CMD_PRINT:
    return cmd_print(args);
  case CMD_PART:
    return cmd_part(args);
  case CMD_BUS:
    return cmd_bus(args);
  case CMD_INSTRUCTION:
    return cmd_instruction(args);
  case CMD_SHIFT:
    return cmd_shift(args);
  case CMD_DR:
    return cmd_dr(args);
  case CMD_GET:
    return cmd_get(args);
  case CMD_SET:
    return cmd_set(args);
  case CMD_ENDIAN:
    return cmd_endian(args);
  case CMD_PEEK:
    return cmd_peek(args);
  case CMD_POKE:
    return cmd_poke(args);
  case CMD_FLASHDETECT:
    return cmd_flashdetect(args);
  case CMD_FLASHREAD:
    return cmd_flashread(args);
  case CMD_FLASHBURN:
    return cmd_flashburn(args);
  case CMD_FLASHERASE:
    return cmd_flasherase(args);
  case CMD_FLASHPROTECT:
    return cmd_flashprotect(args);
  case CMD_SCRIPT:
    return cmd_script(args);
  case CMD_INCLUDE:
    return cmd_include(args);
  case CMD_HELP:
    return cmd_help(args);
  case CMD_UNKNOWN:
  default:
    message("Unknown command : \"%s\"", cmd_line.c_str());
    return -1;
 }
 return 0;
}

int Tap::parse_definition_file(Part *p)
{
 string fname = (string)DATABASE_PATH + (string)"/" +
  (string)p->manufacturer_dir() + (string)"/" + (string)p->part_dir() +
  (string)"/" + (string)p->filename();
 return parse_definition_file(fname.c_str());
}

int Tap::parse_definition_file(const char *fname)
{
 ifstream f(fname, ios::in | ios::binary);
 if (!f) {
  message("Error opening %s file", fname);
  return -1;
 }
 string line;
 int lines = 0;
 while (!f.eof()) {
  getline(f, line);
  lines++;
  if (line[0] == '#')
   continue;
  while (isspace(line[0]))
   line.erase(0, 1);
  if (!line.size())
   continue;
  int res;
  if ((res = run(line))) {
    message("Error (%d) parsing file %s, line %d", res, fname, lines);
    return -1;
  }
 }
 return 0;
}

int Tap::get_signal(Part *p, signal *s)
{
 if (!p || !s)
  return -1;
 data_register *bsr = p->find_data_register("BSR");
 if (!bsr) {
  message("Boundary Scan Register (BSR) not found");
  return -2;
 }
 if (!s->input) {
    message("signal \'%s\' is not input signal", s->name());
    return -3;
 }
  return bsr->out->operator[](s->input->nbit());
}

int Tap::set_signal(Part *p, signal *s, int o, int v)
{
 if (!p || !s)
  return -1;
 data_register *bsr = p->find_data_register("BSR");
 if (!bsr) {
  message("Boundary Scan Register (BSR) not found");
  return -2;
 }
 // setup signal
 if (o) {
  if (!s->output) {
    message("signal \'%s\' cannot be set as output", s->name());
    return -3;
  }
    bsr->in->operator[](s->output->nbit()) = (v & 1);
    int control = (p->get_bit(s->output->nbit()))->control;
    if (control >= 0)
      bsr->in->operator[](control) =
        (p->get_bit(s->output->nbit())->control_value ^ 1);
 } else {
   if (!s->input) {
      message("signal \'%s\' is not input signal", s->name());
      return -3;
   }
   if (s->output)
     bsr->in->operator[](s->output->control) =
      p->get_bit(s->output->nbit())->control_value;
 }
 return 0;
}
