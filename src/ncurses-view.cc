#include "ncurses-view.h"
#include "state.h"
#include <algorithm>
#include <cmath>
#include "highlight.h"
#include <utility>
#include <string>
#include <assert.h>
#include <iostream>

NCursesView::NCursesView(Vim &vm): vm{&vm}, screenStart{0}, screenEnd{screenHeight() - 1}, startUp{true}, scanFile{std::make_unique<Scanner>()} {}

void NCursesView::displayView() {
  refreshWindow();
}

std::pair<int,int> NCursesView::windowSize() {
  return std::make_pair(screenWidth(), screenHeight());
}

int NCursesView::getLength(const std::string &s) const {
  int length = 0;
  int lengthOfTab;
  for (size_t i = 0; i < s.size(); ++i) {
    if (s[i] == '\t') {
      lengthOfTab = (length / TAB_LENGTH + 1) * TAB_LENGTH - length;
      length += lengthOfTab;
    }
    else {
      ++length;
    }
  }
  if (vm->getState() == State::INSERT_MODE && s[s.size() - 1] == '\t') {
    length -= (lengthOfTab - 1);
  }
  return length;
}

void NCursesView::updateStatusBar() {
  std::pair<int,int> rowColDisplay = std::make_pair(screenWidth() - 15, screenHeight() - 1);
  int tmpX = xCoord();
  int tmpY = yCoord();

  //row + col info
  moveCursor(rowColDisplay.second, rowColDisplay.first);
  addStringAtCursor(std::to_string(vm->getCursorY() + 1) + "," + std::to_string(vm->getCursorX() + 1));

  //state info
  moveCursor(rowColDisplay.second, 0);
  if (vm->getState() == State::INSERT_MODE) {
    startUp = false;
    vm->getWarningString() = "";
    addStringAtCursor("-- INSERT --");
  }
  else if (vm->getState() == State::FIND_MODE || vm->getState() == State::RFIND_MODE || vm->getState() == State::COMMAND_MODE) {
    startUp = false;
    vm->getWarningString() = "";
    addStringAtCursor(vm->getSearchString());
    moveCursor(rowColDisplay.second, vm->getSearchString().size());
    return;
  }
  else if (vm->getState() == State::REPLACE_MODE) {
    startUp = false;
    vm->getWarningString() = "";
    addStringAtCursor("-- REPLACE --");
  }
  else if (vm->getWarningString() != ""){
    startUp = false;
    addStringAtCursor(vm->getWarningString());
  }
  else if (vm->getState() == State::NORMAL_MODE && startUp) {
    addStringAtCursor("\"" + vm->getFileName() + "\"");
  }


  moveCursor(tmpY, tmpX);
}

void NCursesView::reformatScreen(int coordinate, int bufferSize) {
  if (coordinate > screenEnd - DISPLACEMENT) {
    screenEnd = std::min(bufferSize, coordinate + DISPLACEMENT);
    screenStart = std::max(0, screenEnd - screenHeight() + 1);
  }
  if (coordinate < screenStart + DISPLACEMENT) {
    screenStart = std::max(0, coordinate - DISPLACEMENT);
    screenEnd = std::min(bufferSize, screenStart + screenHeight() - 1);
  }
}

void NCursesView::update() {
  const std::vector<std::string> fileBuffer = vm->getFileBuffer();
  int tmpY = vm->getCursorY();
  int tmpX = vm->getCursorX();
  Highlight hl = vm->getHighlight();

  bool showSyntaxHighlighting = vm->isCPlusPlusFile();
  std::vector<Scanner::Node> syntax;

  if (hl.getLine() == -1) {
    reformatScreen(tmpY, fileBuffer.size());
  }
  else {
    reformatScreen(hl.getLine(), fileBuffer.size());
  }

  clearScreen();
  for(int i = screenStart; i < screenStart + screenHeight() - 1; ++i) {
    moveCursor(i - screenStart, 0);
    if (i < static_cast<int>(fileBuffer.size())) {
      if (showSyntaxHighlighting) {
        syntax = scanFile->scan(fileBuffer[i]);
      }
      if (hl.getLine() == i) {
        addStringAtCursor(syntax, fileBuffer[i], hl.getLeft(), hl.getRight());
      }
      else {
        addStringAtCursor(syntax, fileBuffer[i]);
      }
      syntax.clear();
    }
    else {
      addStringAtCursor("~");
    }
  }
  int cursorPosition = getLength(fileBuffer[tmpY].substr(0, tmpX + 1)) - 1;
  if (tmpX == static_cast<int>(fileBuffer[tmpY].size())) {
    ++cursorPosition;
  }
  tmpX = std::max(0, cursorPosition);
  moveCursor(tmpY - screenStart, tmpX);
}
