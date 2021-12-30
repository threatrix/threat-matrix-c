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

#ifndef	_REGISTER_H_
#define	_REGISTER_H_

#include <string>
#include <string.h>

using namespace std;

class tap_register {
  char *data_;
  int len_;
  string *str_;
    
  public:
    tap_register(int l, int v = 0) {
     if (l < 1) {
      data_ = 0;
      len_ = 0;
      str_ = 0;
      // throw some error ?
      return;
    }
     len_ = l;
     data_ = new char[len_];
     str_ = 0;
     fill(v);
   }
    ~tap_register() { if (data_) delete [] data_; if (str_) delete str_; }
    tap_register *dup() {
      tap_register *tr = new tap_register(len_);
      tr->set((strval())->c_str());
      return tr;
   }
    int len() const { return len_; }
    char &operator[](int i) { return data_[i]; }
    void copy(tap_register &r, int from, int to) {
      if ((to - from + 1) != len_)
        return;
      memcpy(data_, &(r.data_[from]), to - from + 1);
   }
    tap_register *fill(int val) { memset(data_, val & 1, len_); return this; }
    string *strval() {
      if (str_)
        delete str_;
      str_ = new string(len_, '0');
      for (int i = 0; i < len_; i++)
        (*str_)[len_ - 1 - i] = (data_[i] & 1) ? '1' : '0';
      return str_;
   }
    tap_register *set(const char *value) {
      if (!value)
       return this;
      const char *p = strchr(value, '\0');
      for (int i = 0; i < len_; i++) {
        if (p == value)
          data_[i] = 0;
        else
          data_[i] = (*(--p) == '0') ? 0 : 1;
    }
     return this;
   }
    int cmp(const tap_register &tr) const {
      if (tr.len_ != len_)
        return 1;
      for (int i = 0; i < tr.len_; i++)
        if (data_[i] != tr.data_[i])
          return 1;
      return 0;
   }
    int match(const char *expr) {
      if (!expr || (len_ != (int)strlen(expr)))
        return 0;
      strval();
      for (int i = 0; i < len_; i++)
        if ((expr[i] != '?') && (expr[i] != ((*str_)[i])))
          return 0;
      return 1;
   }
    tap_register *inc() {
      for (int i = 0; i < len_; i++) {
        data_[i] ^= 1;
      if (data_[i] == 1)
        break;
    }
      return this;
   }
    tap_register *dec() {
      for (int i = 0; i < len_; i++) {
        data_[i] ^= 1;
      if (data_[i] == 0)
        break;
    }
      return this;
   }
    tap_register *shift_right(int shift) {
      if (shift < 1)
        return this;
      for (int i = 0; i < len_; i++) {
        if ((i + shift) < len_)
          data_[i] = data_[i + shift];
        else
          data_[i] = 0;
     }
      return this;
   }
    tap_register *shift_left(int shift) {
      if (shift < 1)
        return this;
      for (int i = (len_ - 1); i >= 0; i--) {
        if ((i - shift) >= 0)
          data_[i] = data_[i - shift];
        else
          data_[i] = 0;
     }
      return this;
   }
};

class data_register {

  string name_;

  public:

  tap_register *in, *out;
    data_register(const char *n, int len) : name_(n) {
      in = new tap_register(len);
      out = new tap_register(len);
   }
    ~data_register() { delete in; delete out; }
    const char *name() { return name_.c_str(); }
};

#endif
