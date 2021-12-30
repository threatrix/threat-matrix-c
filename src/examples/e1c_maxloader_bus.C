
#include <fjtag.H>

e1c_maxloader_bus::e1c_maxloader_bus(Tap *t) : Bus("e1c_maxloader bus", t)
{
 char buf[128];
 for (int i = 0; i < 20; i++) {
   sprintf(buf, "a%d", i);
   ex_addr[i] = tap->find_signal(buf);
   if (!ex_addr[i])
     message("e1c_maxloader_bus: signal \'%s\' not found", buf);
 }
 for (int i = 0; i < 16; i++) {
   sprintf(buf, "d%d", i);
   ex_data[i] = tap->find_signal(buf);
   if (!ex_data[i])
     message("e1c_maxloader_bus: signal \'%s\' not found", buf);
 }
 ex_wr = tap->find_signal("wr");
 if (!ex_wr)
   message("e1c_maxloader_bus: signal \'wr\' not found");
 ex_cef0 = tap->find_signal("cef0");
 if (!ex_cef0)
   message("e1c_maxloader_bus: signal \'cef0\' not found");
 ex_oef0 = tap->find_signal("oef0");
 if (!ex_oef0)
   message("e1c_maxloader_bus: signal \'oef0\' not found");
 area(0);
}

void e1c_maxloader_bus::select_flash()
{
 ex_cef0->set(1, 0);
}

void e1c_maxloader_bus::unselect_flash()
{
 ex_cef0->set(1, 1);
}

void e1c_maxloader_bus::prepare()
{
 tap->set_instruction("EXTEST");
 tap->chain_shift_instructions();
}

void e1c_maxloader_bus::read_start(uint32_t a)
{
 select_flash();
 ex_wr->set(1, 1);
 ex_oef0->set(1, 0);
 addr(a);
 data();
 tap->chain_shift_data_registers(0);
}

uint32_t e1c_maxloader_bus::read_next(uint32_t a)
{
 addr(a);
 tap->chain_shift_data_registers(1);
 uint32_t d = 0;
 for (register int i = 0; i < 16; i++)
  d |= (uint32_t)(ex_data[i]->get() << i);
 return d;
}

uint32_t e1c_maxloader_bus::read_end()
{
 unselect_flash();
 ex_oef0->set(1, 1);
 ex_wr->set(1, 1);
 tap->chain_shift_data_registers(1);
 uint32_t d = 0;
 for (register int i = 0; i < 16; i++)
  d |= (uint32_t) (ex_data[i]->get() << i);
 return d;
}

uint32_t e1c_maxloader_bus::read(uint32_t a)
{
 read_start(a);
 return read_end();
}

void e1c_maxloader_bus::write(uint32_t a, uint32_t d)
{
 select_flash();
 addr(a);
 data(d);
 tap->chain_shift_data_registers(0);
 ex_wr->set(1, 0);
 tap->chain_shift_data_registers(0);
 ex_wr->set(1, 1);
 unselect_flash();
 tap->chain_shift_data_registers(0);
}

bus_area *e1c_maxloader_bus::area(uint32_t a)
{
 area_.description = "e1c_maxloader_bus area";
 area_.start = 0x00000000;
 area_.length = 0x100000;
 area_.width = 16;
 return &area_;
}

void e1c_maxloader_bus::addr(uint32_t a)
{
 for (register int i = 0; i < 20; i++)
  ex_addr[i]->set(1, ((a >> i) & 1));
}

// data in
void e1c_maxloader_bus::data()
{
 for (register int i = 0; i < 16; i++)
  ex_data[i]->set(0, 0);
}

// data out
void e1c_maxloader_bus::data(uint32_t d)
{
 for (register int i = 0; i < 16; i++)
  ex_data[i]->set(1, ((d >> i) & 1));
}
