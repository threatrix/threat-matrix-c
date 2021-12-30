
#ifndef	_NIOS_BUS_H_
#define	_NIOS_BUS_H_

#include <part.H>

class nios_bus : public Bus {
    signal *ex_addr[20];
    signal *ex_data[16];
    signal *ex_nwr;
    signal *ex_ncef0;
    signal *ex_noef0;
    signal *ex_ncer0;
    signal *ex_ncer1;
    signal *ex_ncer2;
    signal *ex_ncer3;
    signal *ex_noer0;
    signal *ex_noer1;
    signal *ex_noer2;
    signal *ex_noer3;
  public:
    nios_bus(Tap *t);
    ~nios_bus() { }
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
