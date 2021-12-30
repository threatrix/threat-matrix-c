
#ifndef _ALTERA_H_
#define _ALTERA_H_

#include <cable.H>
#include <parport.H>

class altera_cable_driver : public cable_driver
{
 parport_driver *parport;
 char bb_ii, trst_;

 public:
  altera_cable_driver(const char *device) :
    cable_driver("Altera Byteblaster(II/MV)", device), bb_ii(0) {
  parport = new parport_driver(device);
  check();
 }
  ~altera_cable_driver() { delete parport; }
  void clock(int tms, int tdi);
  int tdo();
  int trst(int) { return 1; }
  int trst() { return trst_; }
  int check();
  const char *dev_error() { return parport->get_error(); }
  int dev_errno() { return parport->get_errno(); }
};

#endif
