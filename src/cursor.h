#ifndef CURSOR_H
#define CURSOR_H

#include "buffer.h"

class Cursor{
  int cursorX, cursorY;
  Buffer *file;

  void moveCursorLeftSingle();
  void moveCursorRightSingle();
  void moveCursorUpSingle();
  void moveCursorDownSingle();
 public:
  Cursor(Buffer *file);
  ~Cursor() = default;
  int getX() const;
  int getY() const;
  void moveCursorLeft(int num = 1);
  void moveCursorRight(int num = 1);
  void moveCursorUp(int num = 1);
  void moveCursorDown(int num = 1);

  void moveCursorX(int pos);
  void moveCursorY(int pos);

  void adjust();
};

#endif
