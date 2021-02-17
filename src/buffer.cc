#include "buffer.h"
#include "history.h"
#include "history-item.h"
#include <string>
#include <assert.h>
#include <iostream>

Buffer::Buffer() {}

Buffer::Buffer(const std::string &fileName): file{std::make_unique<File>(fileName)}, lastSearchCharacter{'\0'}, backup{std::make_unique<Backup>()} {
  file->readFile(buffer);
  if (buffer.size() == 0) {
    buffer.push_back("");
  }
}

const std::vector<std::string> &Buffer::getBuffer() const {
  return buffer;
}
const std::string &Buffer::getFileName() const {
  return file->getFileName();
}

File *Buffer::getFilePointer() {
  return file.get();
}

int Buffer::getPercentage(int posY) const {
  return (100 * (posY + 1)) / buffer.size();
}

void Buffer::insertString(char c, int posX, int posY) {
  std::string s(1, c);
  history.addAction(std::make_pair(posX, posY), std::make_pair(posX + 1, posY), Action::DEL);
  buffer[posY].insert(posX, s);
}

void Buffer::insertNewLine(int posX, int posY, bool jump) {
  if (jump) {
    posX = buffer[posY].size();
  }
  history.addAction(std::make_pair(posX, posY), std::make_pair(0, posY + 1), Action::DEL);
  std::string tmp = buffer[posY].substr(posX);
  buffer[posY].erase(posX);
  buffer.insert(buffer.begin() + (posY + 1), tmp);
}

void Buffer::insertNewLineAbove(int posX, int posY) {
  if (posY == 0) {
    std::string tmpString;
    insertNewLine(posX, posY, true);
    tmpString = buffer[posY];
    removeString(posY, 0, buffer[posY].size() - 1);
    for(size_t i = 0; i < tmpString.size(); ++i) {
      insertString(tmpString[i], i, posY + 1);
    }
  }
  else {
    insertNewLine(posX, posY - 1, true);
  }
}

void Buffer::insertStringVector(int posX, int posY, const std::vector<std::string> &stringVector) {
  int end = stringVector.size() + posY;
  for (int j = posY; j < end; ++j) {
    insertNewLine(posX, j, true);
    for (size_t k = 0; k < stringVector[j - posY].size(); ++k) {
      insertString(stringVector[j - posY][k], k, j + 1);
    }
  }
}

bool Buffer::replaceString(char c, int posX, int posY, bool singleTime, int mult) {
  if (singleTime && mult + posX > static_cast<int>(buffer[posY].size())) {
    return false;
  }
  for (int i = 0; i < mult; ++i) {
    if (backup->existsCharacter(posX, posY) || singleTime) {
      removeString(posY, posX, posX);
    }
    insertString(c, posX, posY);
    ++posX;
  }
  return true;
}

bool Buffer::replaceNewLine(int posX, int posY, bool singleTime, int mult) {
  if (singleTime && mult + posX > static_cast<int>(buffer[posY].size())) {
    return false;
  }
  if (singleTime && posX < static_cast<int>(buffer[posY].size())) {
    for (int i = 0; i < mult; ++i) {
      removeString(posY, posX, posX);
    }
    insertNewLine(posX, posY);
  }
  else {
    backup->insertLine(posX, posY);
    insertNewLine(posX, posY);
  }
  return true;
}

std::pair<int,int> Buffer::replaceBackward(int posX, int posY) {
  if (posX == 0) { //TO DO
    backup->removeLine(posX, posY);
    return removeBackward(posX, posY);
  }
  else if (backup->existsCharacter(posX - 1, posY)) {
    replaceString(backup->accessCharacter(posX - 1, posY), posX - 1, posY, false);
    return std::make_pair(posX - 1, posY);
  }
  else {
    return removeBackward(posX, posY);
  }
}

void Buffer::createBackup() {
  backup->createBackup(buffer);
}

void Buffer::clearBackup() {
  backup->clearBackup();
}

void Buffer::removeString(int posY, int l, int r) {
  remove(std::make_pair(l, posY), std::make_pair(r + 1, posY));
}

std::pair<int,int> Buffer::removeLines(int start, int end) {
  std::pair<int,int> offSet = std::make_pair(0,0);
  if(end + 1 >= static_cast<int>(buffer.size())) {
    if(start == 0) {
      if(buffer[0].size() > 0) {
        remove(std::make_pair(0, 0), std::make_pair(buffer[0].size(), 0));
      }
    } else {
      remove(std::make_pair(0, buffer.size() - 1), std::make_pair(buffer[buffer.size() - 1].size(), buffer.size() - 1));
      removeBackward(0, buffer.size() - 1); // this can be easily changed
      offSet = std::make_pair(0, -1);
    }

    if(start < end) {
      remove(std::make_pair(0, start), std::make_pair(0, buffer.size() - 1));
    }
  } else if (!(start == 0 && (buffer.size() == 1))) {
    remove(std::make_pair(0, start), std::make_pair(0, std::min(end + 1, static_cast<int>(buffer.size()))));
  }

  return offSet;
}

void Buffer::removeForward(int posX, int posY, bool characterFlag) {
  if (posX < static_cast<int>(buffer[posY].size())) {
    remove(std::make_pair(posX, posY), std::make_pair(posX + 1, posY));
  }
  else if (posY + 1 < static_cast<int>(buffer.size()) - 1 && posX == static_cast<int>(buffer[posY].size()) && !characterFlag) {
    remove(std::make_pair(posX, posY), std::make_pair(0, posY + 1));
  }
}

std::pair<int,int> Buffer::removeBackward(int posX, int posY, bool characterFlag) {
  if (posX > 0) {
    history.addAction(std::make_pair(posX - 1, posY), Action::INSERT, std::string(1, buffer[posY][posX - 1])); //undo
    buffer[posY].erase(posX - 1, 1);
    return std::make_pair(posX - 1, posY);
  }
  else if (posX == 0 && posY > 0 && !characterFlag) {
    history.addAction(std::make_pair(buffer[posY-1].size(), posY - 1), Action::INSERT, "\n"); //undo
    int cursorPosition = buffer[posY - 1].size();
    buffer[posY - 1] += buffer[posY];
    buffer.erase(buffer.begin() + posY);
    return std::make_pair(cursorPosition, posY - 1);
  }
  else {
    return std::make_pair(0, 0);
  }
}

void Buffer::remove(std::pair<int,int> pos1, std::pair<int,int> pos2) {
  if(pos1.second != pos2.second) {

    history.addAction(pos1, Action::INSERT, buffer[pos1.second].substr(pos1.first));

    for(int i = 1; i < pos2.second - pos1.second; ++i) {
      history.addAction(pos1, Action::INSERT, '\n' + buffer[pos1.second + i]);
    }

    history.addAction(pos1, Action::INSERT, '\n' + buffer[pos2.second].substr(0, pos2.first));

    buffer[pos2.second].erase(buffer[pos2.second].begin(), buffer[pos2.second].begin() + pos2.first);

    std::string temp = buffer[pos2.second];

    buffer.erase(buffer.begin() + pos1.second + 1, buffer.begin() + pos2.second + 1);

    buffer[pos1.second].erase(buffer[pos1.second].begin() + pos1.first, buffer[pos1.second].end());

    buffer[pos1.second] += temp;
  } else {
    history.addAction(pos1, Action::INSERT, buffer[pos1.second].substr(pos1.first, pos2.first - pos1.first));
    buffer[pos1.second].erase(buffer[pos1.second].begin() + pos1.first, buffer[pos1.second].begin() + pos2.first);
  }
}

void Buffer::undoRemove(std::pair<int,int> pos1, std::pair<int,int> pos2) {
  if(pos1.second != pos2.second) {
    buffer[pos2.second].erase(buffer[pos2.second].begin(), buffer[pos2.second].begin() + pos2.first);
    std::string temp = buffer[pos2.second];

    buffer.erase(buffer.begin() + pos1.second + 1, buffer.begin() + pos2.second + 1);
    buffer[pos1.second].erase(buffer[pos1.second].begin() + pos1.first, buffer[pos1.second].end());

    buffer[pos1.second] += temp;
  } else {
    buffer[pos1.second].erase(buffer[pos1.second].begin() + pos1.first, buffer[pos1.second].begin() + pos2.first);
  }
}

void Buffer::setClipboardString(int posY, int l, int r) { //inclusive
  clipboard.clear();
  clipboard.push_back(buffer[posY].substr(l, r - l + 1));
}

void Buffer::setClipboardLines(int start, int end) { //inclusive
  clipboard.clear();
  clipboard.push_back(""); //insert an empty string at the start
  for (int i = start; i <= end; ++i) {
    clipboard.push_back(buffer[i]);
  }
}

void Buffer::setClipboardVector(std::pair<int,int> start, std::pair<int,int> end) { //end is exclusive
  clipboard.clear();
  clipboard.push_back("");
  if (start.first == end.first && start.second == end.second) {
    return;
  }
  if (end.second < start.second) {
    std::swap(start,end);
  }
  else if (end.second == start.second && start.first >= end.first) {
    std::swap(start,end);
  }
  if (start.second == end.second) {
    clipboard.erase(clipboard.begin());
    clipboard.push_back(buffer[start.second].substr(start.first, end.first - start.first));
  }
  else {
    for (int j = start.second; j <= end.second; ++j) {
      if (j == start.second) {
        clipboard.push_back(buffer[j].substr(start.first));
      }
      else if (j == end.second) {
        if (end.first != 0) {
          clipboard.push_back(buffer[j].substr(0, end.first));
        }
      }
      else {
        clipboard.push_back(buffer[j]);
      }
    }
  }
}

void Buffer::paste(int posX, int posY, bool after) {
  if (clipboard.size() == 1) {
    if (after) {
      if (buffer[posY].size() != 0) {
        history.addAction(std::make_pair(posX + 1, posY), std::make_pair(posX + clipboard[0].size() + 1, posY), Action::DEL);
        buffer[posY] = buffer[posY].substr(0, posX + 1) + clipboard[0] + buffer[posY].substr(posX + 1);
      }
      else {
        history.addAction(std::make_pair(posX, posY), std::make_pair(posX + clipboard[0].size(), posY), Action::DEL);
        buffer[posY] = buffer[posY].substr(0, posX) + clipboard[0] + buffer[posY].substr(posX);
      }
    }
    else {
      history.addAction(std::make_pair(posX, posY), std::make_pair(posX + clipboard[0].size(), posY), Action::DEL);
      buffer[posY] = buffer[posY].substr(0, posX) + clipboard[0] + buffer[posY].substr(posX);
    }
  }
  else if (clipboard.size() >= 2) {
    int idx = posY;
    if (after) {
      ++idx;
      history.addAction(std::make_pair(0, idx), std::make_pair(0, idx + clipboard.size() - 1), Action::DEL, std::make_pair(posX, idx - 1));
    } else {
      history.addAction(std::make_pair(0, posY), std::make_pair(0, idx + clipboard.size() - 1), Action::DEL);
    }


    for (size_t j = 1; j < clipboard.size(); ++j) {
      buffer.insert(buffer.begin() + idx, clipboard[j]);
      ++idx;
    }
  }
}

const std::vector<std::string> &Buffer::getClipboard() const {
  return clipboard;
}

void Buffer::setRecentSearch(const std::string &s) {
  recentSearch = s;
}

int Buffer::findMatch(const std::string &s, int lineNumber, int startIdx, int &occurence, bool after) { //returns location
  if (after) {
    for (int k = startIdx + 1; k <= static_cast<int>(buffer[lineNumber].size() - s.size()); ++k) {
      if (buffer[lineNumber].substr(k, s.size()) == s) {
        --occurence;
        if (occurence == 0) {
          return k;
        }
      }
    }
    return -1;
  }
  else {
    for (int k = startIdx - s.size(); k >= 0; --k) {
      if (buffer[lineNumber].substr(k, s.size()) == s) {
        --occurence;
        if (occurence == 0) {
          return k;
        }
      }
    }
    return -1;
  }
}

std::pair<int,int> Buffer::findInBuffer(const std::string &s, int posX, int posY, int mult, bool after, bool setSearch) {
  if (setSearch) {
    setSearchDirection(after);
  }
  if (s.size() == 0) {
    return std::make_pair(-1, -1);
  }
  int occurence = mult;
  if (after) {
    int check = findMatch(s, posY, posX, occurence, after);
    if (check != -1) {
      return std::make_pair(check, posY);
    }
    else {
      for (size_t j = posY + 1; j < buffer.size(); ++j) {
        int check = findMatch(s, j, -1, occurence, after);
        if (check != -1) {
          return std::make_pair(check, j);
        }
      }
      return std::make_pair(-1, -1);
    }
  }
  else {
    int check = findMatch(s, posY, posX, occurence, after);
    if (check != -1) {
      return std::make_pair(check, posY);
    }
    else {
      for (int j = posY - 1; j >= 0; --j) {
        int check = findMatch(s, j, buffer[j].size(), occurence, after);
        if (check != -1) {
          return std::make_pair(check, j);
        }
      }
      return std::make_pair(-1, -1);
    }
  }
}

std::pair<int,int> Buffer::forwardSearch(int posX, int posY) {
  std::pair<int,int> location = findInBuffer(recentSearch, posX, posY, 1, true, false);
  if (location.first == -1 && location.second == -1) {
    location = findInBuffer(recentSearch, 0, 0, 1, true, false);
  }
  return location;
}

std::pair<int,int> Buffer::backwardSearch(int posX, int posY) {
  std::pair<int,int> location = findInBuffer(recentSearch, posX, posY, 1, false, false);
  if (location.first == -1 && location.second == -1) {
    location = findInBuffer(recentSearch, buffer[buffer.size() - 1].size(), buffer.size() - 1, 1, false, false);
  }
  return location;
}

std::pair<int,int> Buffer::findNext(int posX, int posY, int mult) {
  std::pair<int,int> tmpLocation = std::make_pair(posX, posY);
  for (int i = 0; i < mult; ++i) {
    if (tmpLocation.first != -1 && tmpLocation.second != -1) {
      if (searchDirection) {
        tmpLocation =  forwardSearch(tmpLocation.first, tmpLocation.second);
      }
      else {
        tmpLocation =  backwardSearch(tmpLocation.first, tmpLocation.second);
      }
    }
    else {
      break;
    }
  }
  return tmpLocation;
}

std::pair<int,int> Buffer::findPrev(int posX, int posY, int mult) {
  std::pair<int,int> tmpLocation = std::make_pair(posX, posY);
  for (int i = 0; i < mult; ++i) {
    if (tmpLocation.first != -1 && tmpLocation.second != -1) {
      if (!searchDirection) {
        tmpLocation =  forwardSearch(tmpLocation.first, tmpLocation.second);
      }
      else {
        tmpLocation =  backwardSearch(tmpLocation.first, tmpLocation.second);
      }
    }
    else {
      break;
    }
  }
  return tmpLocation;
}

std::pair<int,int> Buffer::findChar(int posX, int posY, int mult, char c) {
  lastSearchCharacter = c;
  charSearchDirection = true;
  size_t sz = posX;
  for(int i = 0; i < mult; ++i) {
    sz = buffer[posY].find(c, std::min(sz + 1, buffer[posY].size()));
    if (sz == std::string::npos) {
      break;
    }
  }

  if (sz != std::string::npos) {
    return std::make_pair(sz, posY);
  }
  else {
    return std::make_pair(-1, -1);
  }
}

std::pair<int,int> Buffer::rfindChar(int posX, int posY, int mult, char c) {
  lastSearchCharacter = c;
  charSearchDirection = false;
  size_t sz = posX;
  size_t zero = 0;
  for(int i = 0; i < mult; ++i) {
    sz = buffer[posY].rfind(c, std::max(sz - 1, zero));
    if (sz == std::string::npos) {
      break;
    }
  }
  if (sz != std::string::npos) {
    return std::make_pair(sz, posY);
  }
  else {
    return std::make_pair(-1, -1);
  }
}

std::pair<int,int> Buffer::findRecentlyUsedChar(int posX, int posY, int mult) {
  if (charSearchDirection) {
    return findChar(posX, posY, mult, lastSearchCharacter);
  }
  else {
    return rfindChar(posX, posY, mult, lastSearchCharacter);
  }
}

bool Buffer::getCharSearchDirection() const {
  return charSearchDirection;
}

bool Buffer::getSearchDirection() const {
  return searchDirection;
}

void Buffer::setSearchDirection(bool direction) {
  searchDirection = direction;
}

int findWhiteSpace(const std::string &s, int start) {
  for (size_t i = start; i < s.size(); ++i) {
    if (isspace(s[i])) {
      return i;
    }
  }
  return -1;
}

std::pair<int,int> Buffer::moveBackOneWord(int posX, int posY) {
  while (posX > 0 || posY > 0) {
    if (posX == -1) {
      break;
    }
    if (!isspace(buffer[posY][posX])) {
      if (posX == 0) {
        break;
      }
      else if (isspace(buffer[posY][posX - 1])) {
        break;
      }
      else {
        --posX;
      }
    }
    else {
      if (posX == 0) {
        --posY;
        posX = buffer[posY].size() - 1;
      }
      else {
        --posX;
      }
    }
  }
  if (posX == -1) {
    posX = 0;
  }
  return std::make_pair(posX, posY);
}

std::pair<int,int> Buffer::moveForwardOneWord(int posX, int posY) {
  while (posY < static_cast<int>(buffer.size())) {
    if (posY == static_cast<int>(buffer.size()) - 1) {
      if (posX == std::max(0, static_cast<int>(buffer[posY].size()) - 1)) {
        break;
      }
      else {
        if (isspace(buffer[posY][posX])) {
          ++posX;
        }
        else {
          break;
        }
      }
    }
    else {
      if (buffer[posY].size() == 0) {
        break;
      }
      else if (isspace(buffer[posY][posX])) {
        if (posX == static_cast<int>(buffer[posY].size()) - 1) {
          ++posY;
          posX = 0;
        }
        else {
          ++posX;
        }
      }
      else {
        break;
      }
    }
  }
  return std::make_pair(posX, posY);
}

std::pair<int,int> Buffer::moveBackwardByWords(int posX, int posY, int numWords) {
  std::pair<int,int> newLocation = std::make_pair(posX, posY);
  for (int i = 0; i < numWords; ++i) {
    if (newLocation.first == 0 && newLocation.second > 0) {
      newLocation = moveBackOneWord(static_cast<int>(buffer[newLocation.second - 1].size()) - 1, newLocation.second - 1);
    }
    else if (newLocation.first > 0) {
      newLocation = moveBackOneWord(newLocation.first - 1, newLocation.second);
    }
  }
  return newLocation;
}

std::pair<int,int> Buffer::moveForwardByWords(int posX, int posY, int numWords) {
  std::pair<int,int> newLocation = std::make_pair(posX, posY);
  for (int i = 0; i < numWords; ++i) {
    int pos = findWhiteSpace(buffer[newLocation.second], std::max(0, newLocation.first));
    if (pos == -1) {
      if (newLocation.second < static_cast<int>(buffer.size() - 1)) {
        newLocation = moveForwardOneWord(0, newLocation.second + 1);
      }
      else {
        newLocation = std::make_pair(std::max(0, static_cast<int>(buffer[newLocation.second - 1].size()) - 1), newLocation.second);
      }
    }
    else {
      newLocation = moveForwardOneWord(pos, newLocation.second);
    }
  }
  return newLocation;
}

std::pair<int,int> Buffer::moveToNonblank(int posY) {
  for(size_t i = 0; i < buffer[posY].size(); ++i) {
    if (!isspace(buffer[posY][i]) || i == buffer[posY].size() - 1) {
      return std::make_pair(i, posY);
    }
  }
  assert(buffer[posY].size() == 0);
  return std::make_pair(0, posY);
}

std::pair<int,int> Buffer::joinLines(int posX, int posY, int mult) {
  std::string tmpString = buffer[posY];
  std::pair<int,int> cursorPosition = std::make_pair(posX, posY);
  bool joined = false;
  for(int i = posY + 1; i < std::min(static_cast<int>(buffer.size()) - 1, posY + mult); ++i) {
    joined = true;
    if (!isspace(tmpString[tmpString.size() - 1])) {
      tmpString += " ";
    }
    cursorPosition.first = tmpString.size() - 1;
    tmpString += buffer[i];
  }
  if (joined) {
    removeString(posY, 0, buffer[posY].size() - 1);
    for (size_t i = 0; i < tmpString.size(); ++i) {
      insertString(tmpString[i], i, posY);
    }
    removeLines(posY + 1, std::min(static_cast<int>(buffer.size()) - 1, posY + mult) - 1);
  }
  return cursorPosition;
}

std::pair<int,int> Buffer::jumpToEnd() {
  return moveToNonblank(std::max(0, static_cast<int>(buffer.size()) - 1));
}

std::pair<int,int> Buffer::jumpToLine(int lineNumber) {
  if (lineNumber <= 0) {
    return moveToNonblank(0);
  }
  else {
    return moveToNonblank(std::min(lineNumber - 1, static_cast<int>(buffer.size()) - 1));
  }
}

void Buffer::updateHistory() {
  history.updateActions();
}

std::pair<int,int> Buffer::undo() {
  std::vector<HistoryItem> hist = history.getUndo();
  std::pair<int,int> cursorPosition = {-1, -1};

  if(hist.size() > 0) {
    cursorPosition = hist[0].getCursorPosition();

    for(int i = hist.size() - 1; i >= 0; --i) {
      std::pair<int,int> pos = hist[i].getStartPosition();
      Action action = hist[i].getAction();

      if(action == Action::INSERT) {
        std::string s = hist[i].getString();

        if(s[0] == '\n') {
          const std::string temp = buffer[pos.second].substr(pos.first);
          buffer[pos.second].erase(buffer[pos.second].begin() + pos.first, buffer[pos.second].end());
          buffer.insert(buffer.begin() + pos.second + 1, temp);
          buffer[pos.second + 1].insert(0, s.substr(1));
        } else {
          buffer[pos.second].insert(pos.first, hist[i].getString());
        }

      } else {
        undoRemove(pos, hist[i].getEndPosition());
      }
    }

    history.deleteLastUndo();
  }

  return cursorPosition;
}
