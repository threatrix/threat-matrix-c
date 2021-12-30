
#ifndef Fl_History_Input_H
#define Fl_History_Input_H

#include <Fl/Fl_Input.H>
#include <string>
#include <vector>

using namespace std;

class Fl_History_Input : public Fl_Input {

  vector <string> history;
  unsigned int cur_pos, no_doubles;

  void remove_doubles(string &input);
public:
  void doubles(bool d) { no_doubles = d; }
  FL_EXPORT Fl_History_Input(int x, int y, int w, int h, const char *l = 0) :
    Fl_Input(x, y, w, h, l) { no_doubles = cur_pos = 0; }
  FL_EXPORT int handle(int event);
protected:
  void add_to_history();
};

#endif
