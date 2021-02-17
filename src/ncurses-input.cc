#include <ncurses.h>
#include "ncurses-input.h"

int NcursesInput::getChar() const {
  return getch();
}

int NcursesInput::getUpKey() const {
  return KEY_UP;
}

int NcursesInput::getDownKey() const {
  return KEY_DOWN;
}

int NcursesInput::getLeftKey() const {
  return KEY_LEFT;
}

int NcursesInput::getRightKey() const {
  return KEY_RIGHT;
}

int NcursesInput::getEscapeKey() const {
  return 27;
}

int NcursesInput::getBackspaceKey() const {
  return KEY_BACKSPACE;
}

int NcursesInput::getDelKey() const {
  return KEY_DC;
}

int NcursesInput::getCtrlB() const {
  return 2;
}

int NcursesInput::getCtrlD() const {
  return 4;
}

int NcursesInput::getCtrlF() const {
  return 6;
}

int NcursesInput::getCtrlG() const {
  return 7;
}

int NcursesInput::getCtrlU() const {
  return 21;
}
