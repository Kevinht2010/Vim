#include "keyboard.h"
#include <cctype>
#include <assert.h>

Keyboard::Keyboard(VimState *vs): vs{vs}{
  keyboardMap['i'] = Action::INSERT;
  keyboardMap['I'] = Action::INSERT_AT_BEGINNING;

  keyboardMap['y'] = Action::COPY;
  keyboardMap['c'] = Action::CHANGE;
  keyboardMap['d'] = Action::DEL;

  keyboardMap['p'] = Action::PASTE_AFTER;
  keyboardMap['P'] = Action::PASTE_BEFORE;

  keyboardMap['h'] = Action::CURSOR_LEFT;
  keyboardMap['j'] = Action::CURSOR_DOWN;
  keyboardMap['k'] = Action::CURSOR_UP;
  keyboardMap['l'] = Action::CURSOR_RIGHT;

  keyboardMap['/'] = Action::FIND;
  keyboardMap['?'] = Action::RFIND;
  keyboardMap['n'] = Action::FIND_NEXT;
  keyboardMap['N'] = Action::FIND_PREV;
  keyboardMap['f'] = Action::FIND_CHARACTER;
  keyboardMap['F'] = Action::RFIND_CHARACTER;

  keyboardMap['u'] = Action::UNDO;
  keyboardMap['.'] = Action::LAST_ACTION;

  keyboardMap['A'] = Action::END_OF_LINE_INSERT;
  keyboardMap['a'] = Action::NEXT_CHARACTER_INSERT;

  keyboardMap['r'] = Action::REPLACE_CHARACTER;
  keyboardMap['R'] = Action::REPLACE;

  keyboardMap['S'] = Action::REMOVE_LINE;
  keyboardMap['s'] = Action::REMOVE_CHARACTER;

  keyboardMap['b'] = Action::PREV_WORD;
  keyboardMap['w'] = Action::NEXT_WORD;

  keyboardMap['0'] = Action::MOVE_TO_FRONT;
  keyboardMap['$'] = Action::MOVE_TO_END;

  keyboardMap['^'] = Action::MOVE_TO_NONBLANK;
  keyboardMap[';'] = Action::EXECUTE_FINDCHAR;

  keyboardMap['J'] = Action::JOIN;

  keyboardMap['x'] = Action::DEL_KEY;
  keyboardMap['X'] = Action::BACKSPACE;

  keyboardMap['o'] = Action::INSERT_LINE_BELOW;
  keyboardMap['O'] = Action::INSERT_LINE_ABOVE;

  keyboardMap[':'] = Action::COMMAND;
}

std::pair<Action, int> Keyboard::action() {
  int c;
  c = getChar();
  if (vs->getState() != State::INSERT_MODE && vs->getState() != State::FIND_MODE && vs->getState() != State::RFIND_MODE
                                           && vs->getState() != State::REPLACE_MODE && vs->getState() != State::COMMAND_MODE
                                           && vs->getState() != State::FIND_CHARACTER_MODE && vs->getState() != State::RFIND_CHARACTER_MODE
                                           && vs->getState() != State::REPLACE_CHARACTER_MODE) {
    if (c == getUpKey()) {
      return std::make_pair(Action::CURSOR_UP, 0);
    }
    else if (c == getDownKey()) {
      return std::make_pair(Action::CURSOR_DOWN, 0);
    }
    else if (c == getLeftKey()) {
      return std::make_pair(Action::CURSOR_LEFT, 0);
    }
    else if (c == getRightKey()) {
      return std::make_pair(Action::CURSOR_RIGHT, 0);
    }
    else if (c == getCtrlB()) {
      return std::make_pair(Action::PAGE_BACK, 0);
    }
    else if (c == getCtrlD()) {
      return std::make_pair(Action::HALF_PAGE_FORWARD, 0);
    }
    else if (c == getCtrlF()) {
      return std::make_pair(Action::PAGE_FORWARD, 0);
    }
    else if (c == getCtrlG()) {
      return std::make_pair(Action::DISPLAY_STATS, 0);
    }
    else if (c == getCtrlU()) {
      return std::make_pair(Action::HALF_PAGE_BACK, 0);
    }
    else if ((c >= '1' && c <= '9') || (c == '0' && vs->getState() == State::MULT_MODE)) {
      return std::make_pair(Action::MULT, c);
    }
    else if (keyboardMap.find(c) != keyboardMap.end()) {
      return std::make_pair(keyboardMap.at(c), 0);
    }
    else {
      return std::make_pair(Action::NOTHING, 0);
    }
  } else {
    if (c == getEscapeKey()) {
      return std::make_pair(Action::ESCAPE, 0);
    }
    else if (c == getBackspaceKey()) {
      return std::make_pair(Action::BACKSPACE, 0);
    }
    else if (c == getDelKey()) {
      return std::make_pair(Action::DEL_KEY, 0);
    }
    else if(isascii(c)){
      if (isprint(c))
        return std::make_pair(Action::CHARACTER, c);
      else if (c == '\n')
        return std::make_pair(Action::NEWLINE, 0);
      else if (c == '\t')
        return std::make_pair(Action::TAB, 0);
      else
        return std::make_pair(Action::NOTHING, 0);
    }
    else {
      return std::make_pair(Action::NOTHING, 0);
    }
  }
}

Keyboard::~Keyboard() {}
