
#include <cable.H>

/*
 * data D[7:0] (pins 9:2)
 */

#define TDI             6
#define TCK             0
#define TMS             1
#define BB_CHECK        5

/*
 * 7 - BUSY (pin 11)
 * 6 - ACK (pin 10)
 * 5 - PE (pin 12)
 * 4 - SEL (pin 13)
 * 3 - ERROR (pin 15)
 */

#define TDO             7
#define BB_PRESENT      6
#define VCC_OK_N        3

/*
 * 0 - STROBE (pin 1)
 * 1 - AUTOFD (pin 14)
 * 2 - INIT (pin 16)
 * 3 - SELECT (pin 17)
*/

#define BB_ENABLE       0xC

int altera_cable_driver::check()
{
 if (parport->device_status() == CLOSED_STATE) {
   if (parport->open())
     return status(DEVICE_ERROR);
  }
 if (parport->device_status() == ERROR_STATE) {
  parport->close();
  return status(DEVICE_ERROR);
 }
 trst_ = 1;
 // check if a ByteBlaster or ByteBlasterMV is connected
 parport->set_data(1 << BB_CHECK);
 unsigned char s;
 int res = parport->status(s);
 if (res) {
  parport->close();
  return status(DEVICE_ERROR);
 }
 if (!((s >> BB_PRESENT ) & 1 ))
  bb_ii = 1;
 parport->set_data(0);
 res = parport->status(s);
 if (res) {
  parport->close();
  return status(DEVICE_ERROR);
 }
 if ((s >> BB_PRESENT ) & 1)
  bb_ii = 1;
 // check if the power supply is ok (only for ByteBlaster II)
 // if no ByteBlaster at all is connected this check will fail, too
 if (bb_ii) {
   res = parport->status(s);
   if (res) {
     parport->close();
     return status(DEVICE_ERROR);
   }
   if ((s >> VCC_OK_N) & 1)
    return status(CABLE_NOT_PRESENT);
  }
   parport->control(BB_ENABLE);
   return status(CABLE_PRESENT);
}

void altera_cable_driver::clock(int tms, int tdi)
{
 tms = tms ? 1 : 0;
 tdi = tdi ? 1 : 0;
 parport->set_data((0 << TCK) | (tms << TMS) | (tdi << TDI));
 wait();
 parport->set_data((1 << TCK) | (tms << TMS) | (tdi << TDI));
 wait();
}

int altera_cable_driver::tdo()
{
 unsigned char s;

 parport->set_data(0 << TCK);
 wait();
 parport->status(s);
 return ((s >> TDO) & 1);
}
