/*
 * Copyright (C) 2002 ETC s.r.o.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Written by Marcel Telka <marcel@telka.sk>, 2002.
 * Rewritten in c++ by Rus V. Brushkoff <rus@soyuzkt.od.ua>, 2004.
 *
 */

#ifndef	_BUS_H_
#define	_BUS_H_

#include <part.H>
#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

class bus_area {
  public:
    string description;
    uint32_t start;
    uint64_t length;
    int width;
};

#include <flash.H>

class Tap;

// Hmm, int, even unsigned, is not a type for generic bus (use BigInt ?!) !

class Bus {

  string name_;

  public:
  bus_area area_;
  Tap *tap; // bus may consists from several parts, so ...
  flash_driver *flash; // but must have only one flash ;(
  
    Bus(const char *n, Tap *t) : name_(n), tap(t), flash(0) { }
    virtual ~Bus() { }
    virtual void prepare() = 0;
    virtual void read_start(uint32_t a) = 0;
    virtual uint32_t read_next(uint32_t a) = 0;
    virtual uint32_t read_end() = 0;
    virtual uint32_t read(uint32_t a) = 0;
    virtual void write(uint32_t a, uint32_t) = 0;
    virtual bus_area *area(uint32_t a) = 0;
    virtual void addr(uint32_t a) = 0;
    virtual void data() = 0; // data_in
    virtual void data(uint32_t d) = 0; // data_out

    const char *name() { return name_.c_str(); }
    int width() { return area_.width; }
    void print_flash_info();
};

static inline void delete_bus(Bus *b) { delete b; }

#define BUS_UNKNOWN         0
#define BUS_E1C_MAXLOADER   1
#define BUS_NIOS            2

#include <e1c_maxloader_bus.H>
#include <nios_bus.H>

#endif
