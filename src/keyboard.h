#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "action.h"
#include <utility>
#include <string>
#include <map>
#include "ncurses-input.h"
#include "controller.h"
#include "vim-state.h"

class Keyboard : public Controller, public NcursesInput {
  VimState *vs;
  std::map<char, Action> keyboardMap;
  std::pair<Action, int> action() override;
 public:
  Keyboard(VimState *vs);

  virtual ~Keyboard();
};

#endif
