
#ifndef _PORT_H_
#define _PORT_H_

class port {

 public:

  port();
  ~port();
  int open();
  int close();
  int set_data(uint8_t data);
  int get_data();
  int get_status();
  int set_control(uint8_t data);


};

// extern parport_driver_t *parport_drivers[];

#endif
