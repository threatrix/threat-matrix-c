
#ifndef _CABLE_H_
#define _CABLE_H_

#include <string>
#include <unistd.h>

using namespace std;

#define CABLE_NOT_PRESENT 0
#define CABLE_PRESENT     1
#define DEVICE_ERROR      2

#define UNKNOWN_CABLE       0
#define ALTERA_BYTEBLASTER  1

class cable_driver {

  string name_;
  string device_;
  int frequency_, cable_status_;
   
 public:
  cable_driver(const char *n, const char *d) :
    name_(n), device_(d), frequency_(0), cable_status_(CABLE_NOT_PRESENT) { }
  virtual ~cable_driver() { }
  virtual void clock(int tms, int tdi) { }
  virtual int tdo() { return -1; }
  virtual int trst(int) { return -1; }
  virtual int trst() { return -1; }
  virtual int check() { return -1; }
  virtual const char *dev_error() = 0;
  virtual int dev_errno() { return DEVICE_ERROR; };
  int status() { return cable_status_; }
  int status(int st) { return (cable_status_ = st); }
  const char *dev_name() { return device_.c_str(); }
  const char *name() { return name_.c_str(); }
  void frequency(int freq) { frequency_ = freq; }
  int frequency() { return frequency_; }
  void wait() {
   if (!frequency_)
    return;
   int period = 1000000 / frequency_ / 2;
   if (!period)
    period = 1;
   ::usleep(period);
  };
};

class default_cable_driver : public cable_driver {
  public:
    default_cable_driver(const char *dev) : cable_driver("None", dev) { }
    ~default_cable_driver() { }
  const char *dev_error() { return "Unknown cable"; }
};

#include <altera.H>

#endif
