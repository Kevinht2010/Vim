#ifndef VIM_STATE_H
#define VIM_STATE_H

#include "state.h"

class VimState {
  State s;
  int multiplier;
 public:
  VimState();
  void setMultiplier(int v);
  int getMultiplier();
  void setState(State x);
  State getState();
};

#endif
