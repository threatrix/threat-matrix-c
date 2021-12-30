
#ifndef	_E1C_H_
#define	_E1C_H_

#include <part.H>

class e1c_maxloader_bus : public Bus {
    signal *ex_addr[20];
    signal *ex_data[16];
    signal *ex_wr;
    signal *ex_cef0;
    signal *ex_oef0;
  public:
    e1c_maxloader_bus(Tap *t);
    ~e1c_maxloader_bus() { }
    void select_flash();
    void unselect_flash();
    void initbus();
    void prepare();
    void read_start(uint32_t a);
    uint32_t read_next(uint32_t a);
    uint32_t read_end();
    uint32_t read(uint32_t a);
    void write(uint32_t a, uint32_t d);
    bus_area *area(uint32_t a);
    void addr(uint32_t a);
    void data(); // data_in
    void data(uint32_t d); // data_out
};

#endif
