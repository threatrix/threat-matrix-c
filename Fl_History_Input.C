
/*
 *      Copyright (C) 1999  Rus V. Brushkoff
 */

#include "Fl_History_Input.H"
#include <FL/Fl.H>
#include <FL/fl_ask.H>

 /* 
  *   Fl_History_Input - стандартный input с историей
  */
  
FL_EXPORT int Fl_History_Input::handle(int event) {
    switch (event) {
      case FL_KEYBOARD:
        switch (Fl::event_key()) {
          case FL_Right:
            if ((this->mark() - this->position()) == this->size()) {
              this->position(this->mark());
              return 1;
           } else
              break;
          case FL_Enter:
          case FL_KP_Enter:
            add_to_history();
            break;
          case FL_Up: // history up
            if (!cur_pos || !history.size()) {
              fl_beep();
              return 1;
          }
           if (((cur_pos == 1) && !strcmp(this->value(), history[0].c_str())))
             fl_beep();
           this->value(history[cur_pos - 1].c_str());
           this->redraw();
           if (cur_pos != 1)
            cur_pos--;
           return 1;
          case FL_Down: // history down
           if (!cur_pos || !history.size()) {
             fl_beep();
             return 1;
          }
           if ((cur_pos == history.size()) && !this->size()) {
             fl_beep();
             return 1;
         }
           cur_pos++;
           if (history.size() > (cur_pos - 1)) {
             this->value(history[cur_pos - 1].c_str());
             this->redraw();
         } else
            this->value("");
          if (cur_pos > history.size())
            cur_pos = history.size();
          return 1;
     }
 }
  return Fl_Input::handle(event);
}

void Fl_History_Input::remove_doubles(string &line)
{
 vector <string>::iterator iter;
 for (iter = history.begin(); iter != history.end(); iter++)
   if (!strcmp((*iter).c_str(), line.c_str())) {
     history.erase(iter);
     return;
  }
}

void Fl_History_Input::add_to_history()
{
  string line(this->value());
  if (!line.size())
    return;
  if (!no_doubles) // TODO: как всегда все наоборот
    remove_doubles(line);
  history.push_back(line);
  cur_pos = history.size();
}
