#include "ncurses-display.h"
#include <ncurses.h>
#include <cstring>
#include <assert.h>
#include <iostream>
#define ID_PAIR 1
#define STRING_PAIR 2
#define NUM_PAIR 3
#define COMMENT_PAIR 4
#define DIRECTIVE_PAIR 5
#define KEYWORD_PAIR 6
#define ELSE_PAIR 7

NcursesDisplay::NcursesDisplay() {
  initscr();
  noecho();
  start_color();
  keypad(stdscr, TRUE);
  set_escdelay(0);
  init_pair(ID_PAIR, COLOR_YELLOW, COLOR_BLACK);
  init_pair(STRING_PAIR, COLOR_BLUE, COLOR_BLACK);
  init_pair(NUM_PAIR, COLOR_RED, COLOR_BLACK);
  init_pair(COMMENT_PAIR, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(DIRECTIVE_PAIR, COLOR_GREEN, COLOR_BLACK);
  init_pair(KEYWORD_PAIR, COLOR_CYAN, COLOR_BLACK);
  init_pair(ELSE_PAIR, COLOR_WHITE, COLOR_BLACK);
}

int NcursesDisplay::screenHeight() {
  getmaxyx(stdscr, h, w);
  return h;
}

int NcursesDisplay::screenWidth() {
  getmaxyx(stdscr, h, w);
  return w;
}

int NcursesDisplay::xCoord() {
  getyx(stdscr, curYPos, curXPos);
  return curXPos;
}

int NcursesDisplay::yCoord() {
  getyx(stdscr, curYPos, curXPos);
  return curYPos;
}

void NcursesDisplay::addCharAtCursor(char c) {
  addch(c);
}

void NcursesDisplay::addStringAtCursor(const std::vector<Scanner::Node> &syntax, const std::string &s, int highlightLeft, int highlightRight) {
  if (syntax.size() == 0) {
    addStringAtCursor(s, highlightLeft, highlightRight);
  }
  else {
    for (int i = 0; i < static_cast<int>(s.length()); ++i) {
      char arr[2];
      int yPos, xPos;
      strcpy(arr, s.substr(i, 1).c_str());
      if (i == highlightLeft) {
        attron(A_STANDOUT);
      }
      activateColor(syntax[i]);
      getyx(stdscr ,yPos, xPos);
      mvprintw(yPos, xPos, arr);
      if (i == highlightRight) {
        attroff(A_STANDOUT);
      }
      deactivateColor(syntax[i]);
    }
  }
}

void NcursesDisplay::addStringAtCursor(const std::string &s, int highlightLeft, int highlightRight) {
  for (int i = 0; i < static_cast<int>(s.length()); ++i) {
    char arr[2];
    int yPos, xPos;
    strcpy(arr, s.substr(i, 1).c_str());
    if (i == highlightLeft) {
      attron(A_STANDOUT);
    }
    getyx(stdscr ,yPos, xPos);
    mvprintw(yPos, xPos, arr);
    if (i == highlightRight) {
      attroff(A_STANDOUT);
    }
  }
}

void NcursesDisplay::activateColor(Scanner::Node n) {
  if (n == Scanner::Node::ID) {
    attron(COLOR_PAIR(ID_PAIR));
  }
  else if (n == Scanner::Node::STRING) {
    attron(COLOR_PAIR(STRING_PAIR));
  }
  else if (n == Scanner::Node::NUM) {
    attron(COLOR_PAIR(NUM_PAIR));
  }
  else if (n == Scanner::Node::COMMENT) {
    attron(COLOR_PAIR(COMMENT_PAIR));
  }
  else if (n == Scanner::Node::DIRECTIVE) {
    attron(COLOR_PAIR(DIRECTIVE_PAIR));
  }
  else if (n == Scanner::Node::KEYWORD) {
    attron(COLOR_PAIR(KEYWORD_PAIR));
  }
}

void NcursesDisplay::deactivateColor(Scanner::Node n) {
  if (n == Scanner::Node::ID) {
    attroff(COLOR_PAIR(ID_PAIR));
  }
  else if (n == Scanner::Node::STRING) {
    attroff(COLOR_PAIR(n + 1));
  }
  else if (n == Scanner::Node::NUM) {
    attroff(COLOR_PAIR(n + 1));
  }
  else if (n == Scanner::Node::COMMENT) {
    attroff(COLOR_PAIR(n + 1));
  }
  else if (n == Scanner::Node::DIRECTIVE) {
    attroff(COLOR_PAIR(n + 1));
  }
  else if (n == Scanner::Node::KEYWORD) {
    attroff(COLOR_PAIR(n + 1));
  }
}

void NcursesDisplay::moveCursor(int cursorY, int cursorX) {
  move(cursorY, cursorX);
}

void NcursesDisplay::refreshWindow() {
  refresh();
}

void NcursesDisplay::clearScreen() {
  clear();
}

NcursesDisplay::~NcursesDisplay() {
  endwin();
}
