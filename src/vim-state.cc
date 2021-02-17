#include "vim-state.h"

VimState::VimState(): s{State::NORMAL_MODE}, multiplier{0} {}

void VimState::setMultiplier(int v) {
  multiplier = v;
}

int VimState::getMultiplier() {
  return multiplier;
}

void VimState::setState(State x) {
  s = x;
}

State VimState::getState() {
  return s;
}
