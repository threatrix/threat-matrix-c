
#ifndef _PARPORT_H_
#define _PARPORT_H_

#include <fcntl.h>
#include <stropts.h>
#include <unistd.h>
#include <linux/ppdev.h>
#include <linux/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <string>

using namespace std;

#define CLOSED_STATE    0
#define OPENED_STATE    1
#define ERROR_STATE     2

class parport_driver {
  string last_error;
  const char *device;
  int fd, last_errno, dev_status;
 public:
  parport_driver(const char *dev) {
    device = dev;
    fd = -1;
    last_errno = 0;
    dev_status = CLOSED_STATE;
  }
  ~parport_driver() { close(); }
  int open();
  int close();
  int set_data(int data);
  int get_data(unsigned char &data);
  int status(unsigned char &status);
  int control(unsigned char data);
  const char *get_error() { return strerror(last_errno); }
  int get_errno() { return last_errno; }
  int device_status() {
    return (last_errno ? ERROR_STATE : dev_status);
  }
};

#endif
