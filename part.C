
#include <part.H>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <fjtag.H>

int signal::set(int o, int v)
{
 return part->tap->set_signal(part, this, o, v);
}

int signal::get()
{
 return part->tap->get_signal(part, this);
}
        
// format of database :
// bits   dir   full_name

int Part::find_record(string &data_file, tap_register &r, int bits_from,
  int bits_to, string &bits, string &directory, string &full_name)
{
 ifstream f(data_file.c_str(), ios::in | ios::binary);
 if (!f)
  return 0;
 string line;
 tap_register reg(bits_to - bits_from + 1), freg(bits_to - bits_from + 1);
 reg.copy(r, bits_from, bits_to);
 while (!f.eof()) {
   getline(f, line);
   if (line[0] == '#')
    continue;
   while (isspace(line[0]))
    line.erase(0, 1);
   if (!line.size())
    continue;
   string spaces("\t\n\r\f\v"); // " \t\r\n\f\v"
   string::size_type bits_size = line.find_first_of(spaces, 0);
   bits = line.substr(0, bits_size);
   // check register
   if ((int)bits.size() != reg.len()) // check len
    continue;
   freg.set(bits.c_str());
   if (freg.cmp(reg))
    continue;
   string::size_type dir_start = line.find_first_not_of(spaces, bits_size);
   string::size_type dir_end = line.find_first_of(spaces, dir_start);
   directory = line.substr(dir_start, dir_end - dir_start);
   string::size_type full_name_start =
    line.find_first_not_of(spaces, dir_end);
   string::size_type full_name_end =
    line.find_first_of(spaces, full_name_start);
   if (full_name_end == string::npos)
    full_name_end = line.size();
   full_name =
    line.substr(full_name_start, full_name_end - full_name_start);
 }
 return 1;
}

Part::Part(tap_register &r, Tap *t)
{
 manufacturer_ = name_ = stepping_ = filename_ = "Unknown";
 active_instruction_ = 0;
 string data_file((string)DATABASE_PATH + (string)"/" +
  (string)"MANUFACTURERS");
 string s1, s2, s3;
 if (!find_record(data_file, r, 1, 11, s1, s2, s3)) // not found
  return;
 manufacturer_ = s3;
 manufacturer_dir_ = s2;
 data_file = (string)DATABASE_PATH + (string)"/" + manufacturer_dir_ +
  (string)"/" + (string)"PARTS";
 if (!find_record(data_file, r, 12, 27, s1, s2, s3)) // not found
  return;
 name_ = s3;
 part_dir_ = s2;
 data_file = (string)DATABASE_PATH + (string)"/" + manufacturer_dir_ +
  (string)"/" + part_dir_ + (string)"/" + (string)"STEPPINGS";
 if (!find_record(data_file, r, 28, 31, s1, s2, s3)) // not found
  return;
 stepping_ = s3;
 filename_ = s2;
 id = r.dup();
 tap = t;
}

instruction *Part::find_instruction(char const *instr)
{
 for (int i = 0; i < (int)instructions.size(); i++)
  if (!strcmp(instructions[i]->name(), instr))
    return instructions[i];
 return 0;
}

signal *Part::find_signal(const char *sig)
{
 vector <signal *>::iterator iter = signals.begin();
 for (; iter != signals.end(); iter++)
  if (!strcmp((*iter)->name(), sig))
    return (*iter);
 return 0;
}

data_register *Part::find_data_register(const char *dr)
{
 vector <data_register *>::iterator iter = data_registers.begin();
 for (; iter != data_registers.end(); iter++)
  if (!strcmp((*iter)->name(), dr))
    return (*iter);
 return 0;
}
