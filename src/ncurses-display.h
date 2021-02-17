#ifndef NCURSES_DISPLAY_H
#define NCURSES_DISPLAY_H

#include<string>
#include "scanner.h"
class NcursesDisplay {
  int h,w;
  int curXPos;
  int curYPos;
 private:
  void activateColor(Scanner::Node n);
  void deactivateColor(Scanner::Node n);
 protected:
  NcursesDisplay();

  int screenHeight();

  int screenWidth();

  int xCoord();

  int yCoord();

  void refreshWindow();

  void addCharAtCursor(char c);

  void addStringAtCursor(const std::vector<Scanner::Node> &syntax, const std::string &s, int highlightLeft = -1, int highlightRight = -1);
  void addStringAtCursor(const std::string &s, int highlightLeft = -1, int highlightRight = -1);

  void moveCursor(int cursorY, int cursorX);

  void clearScreen();

  ~NcursesDisplay();
};

#endif
