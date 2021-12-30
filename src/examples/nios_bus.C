
#include <fjtag.H>

nios_bus::nios_bus(Tap *t) : Bus("Nios bus", t)
{
 char buf[128];
 for (int i = 0; i < 20; i++) {
   sprintf(buf, "na%d", i);
   ex_addr[i] = tap->find_signal(buf);
   if (!ex_addr[i])
     message("nios_bus: signal \'%s\' not found", buf);
 }
 for (int i = 0; i < 16; i++) {
   sprintf(buf, "nd%d", i);
   ex_data[i] = tap->find_signal(buf);
   if (!ex_data[i])
     message("nios_bus: signal \'%s\' not found", buf);
 }
 ex_nwr = tap->find_signal("nwr");
 if (!ex_nwr)
   message("nios_bus: signal \'nwr\' not found");
 ex_ncef0 = tap->find_signal("ncef0");
 if (!ex_ncef0)
   message("nios_bus: signal \'ncef0\' not found");
 ex_noef0 = tap->find_signal("noef0");
 if (!ex_noef0)
   message("nios_bus: signal \'noef0\' not found");
 ex_noer0 = tap->find_signal("noer0");
 if (!ex_noer0)
   message("nios_bus: signal \'noer0\' not found");
 ex_noer1 = tap->find_signal("noer1");
 if (!ex_noer1)
   message("nios_bus: signal \'noer1\' not found");
 ex_noer2 = tap->find_signal("noer2");
 if (!ex_noer2)
   message("nios_bus: signal \'noer2\' not found");
 ex_noer3 = tap->find_signal("noer3");
 if (!ex_noer3)
   message("nios_bus: signal \'noer3\' not found");
 ex_ncer0 = tap->find_signal("ncer0");
 if (!ex_ncer0)
   message("nios_bus: signal \'ncer0\' not found");
 ex_ncer1 = tap->find_signal("ncer1");
 if (!ex_ncer1)
   message("nios_bus: signal \'ncer1\' not found");
 ex_ncer2 = tap->find_signal("ncer2");
 if (!ex_ncer2)
   message("nios_bus: signal \'ncer2\' not found");
 ex_ncer3 = tap->find_signal("ncer3");
 if (!ex_ncer3)
   message("nios_bus: signal \'ncer3\' not found");
 area(0);
}

void nios_bus::select_flash()
{
 ex_ncer0->set(1, 1);
 ex_ncer1->set(1, 1);
 ex_ncer2->set(1, 1);
 ex_ncer3->set(1, 1);
 ex_noer0->set(1, 1);
 ex_noer1->set(1, 1);
 ex_noer2->set(1, 1);
 ex_noer3->set(1, 1);
 ex_ncef0->set(1, 0);
}

void nios_bus::unselect_flash()
{
 ex_ncef0->set(1, 1);
}

void nios_bus::prepare()
{
 tap->set_instruction("EXTEST");
 tap->chain_shift_instructions();
}

void nios_bus::read_start(uint32_t a)
{
 select_flash();
 ex_nwr->set(1, 1);
 ex_noef0->set(1, 0);
 addr(a);
 data();
 tap->chain_shift_data_registers(0);
}

uint32_t nios_bus::read_next(uint32_t a)
{
 addr(a);
 tap->chain_shift_data_registers(1);
 uint32_t d = 0;
 for (register int i = 0; i < 16; i++)
  d |= (uint32_t)(ex_data[i]->get() << i);
 return d;
}

uint32_t nios_bus::read_end()
{
 unselect_flash();
 ex_noef0->set(1, 1);
 ex_nwr->set(1, 1);
 tap->chain_shift_data_registers(1);
 uint32_t d = 0;
 for (register int i = 0; i < 16; i++)
  d |= (uint32_t) (ex_data[i]->get() << i);
 return d;
}

uint32_t nios_bus::read(uint32_t a)
{
 read_start(a);
 return read_end();
}

void nios_bus::write(uint32_t a, uint32_t d)
{
 select_flash();
 addr(a);
 data(d);
 tap->chain_shift_data_registers(0);
 ex_nwr->set(1, 0);
 tap->chain_shift_data_registers(0);
 ex_nwr->set(1, 1);
 unselect_flash();
 tap->chain_shift_data_registers(0);
}

bus_area *nios_bus::area(uint32_t a)
{
 area_.description = "nios_bus area";
 area_.start = 0x00000000;
 area_.length = 0x100000;
 area_.width = 16;
 return &area_;
}

void nios_bus::addr(uint32_t a)
{
 for (register int i = 0; i < 20; i++)
  ex_addr[i]->set(1, ((a >> i) & 1));
}

// data in
void nios_bus::data()
{
 for (register int i = 0; i < 16; i++)
  ex_data[i]->set(0, 0);
}

// data out
void nios_bus::data(uint32_t d)
{
 for (register int i = 0; i < 16; i++)
  ex_data[i]->set(1, ((d >> i) & 1));
}
