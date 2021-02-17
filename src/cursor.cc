#include "cursor.h"
#include <algorithm>

Cursor::Cursor(Buffer *file): cursorX{0}, cursorY{0}, file{file}  {}

int Cursor::getX() const {
  return cursorX;
}

int Cursor::getY() const {
  return cursorY;
}

void Cursor::moveCursorUpSingle() {
  cursorY = std::max(cursorY - 1, 0);
  adjust();
}

void Cursor::moveCursorDownSingle() {
  int bound = file->getBuffer().size() - 1;
  cursorY = std::min(cursorY + 1, bound);
  adjust();
}

void Cursor::moveCursorLeftSingle() {
  cursorX = std::max(cursorX - 1, 0);
}

void Cursor::moveCursorRightSingle() {
  ++cursorX;
  adjust();
}

void Cursor::moveCursorUp(int num) {
  for(int i = 0; i < num; ++i) {
    moveCursorUpSingle();
  }
}

void Cursor::moveCursorDown(int num) {
  for(int i = 0; i < num; ++i) {
    moveCursorDownSingle();
  }
}

void Cursor::moveCursorLeft(int num) {
  for(int i = 0; i < num; ++i) {
    moveCursorLeftSingle();
  }
}

void Cursor::moveCursorRight(int num) {
  for(int i = 0; i < num; ++i) {
    moveCursorRightSingle();
  }
}

void Cursor::moveCursorX(int pos) {
  cursorX = pos;
}

void Cursor::moveCursorY(int pos) {
  cursorY = pos;
}

void Cursor::adjust() {
  int bound = file->getBuffer()[cursorY].size() - 1;
  cursorX = std::max(0, std::min(cursorX, bound));
}
