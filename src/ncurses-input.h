#ifndef NCURSES_INPUT_H
#define NCURSES_INPUT_H

class NcursesInput {
 protected:
  NcursesInput() = default;

  ~NcursesInput() = default;

  int getChar() const;

  int getUpKey() const;

  int getDownKey() const;

  int getLeftKey() const;

  int getRightKey() const;

  int getEscapeKey() const;

  int getBackspaceKey() const;

  int getDelKey() const;

  int getCtrlB() const;

  int getCtrlD() const;

  int getCtrlF() const;

  int getCtrlG() const;

  int getCtrlU() const;
};

#endif
