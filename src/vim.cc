#include "vim.h"
#include "action.h"
#include "ncurses-view.h"
#include "keyboard.h"
#include <cctype>
#include <cmath>
#include <cstdlib>
using std::max;
using std::min;

Vim::Vim(const std::string &fileName, bool isCPlusPlus): file{std::make_unique<Buffer>(fileName)}, vs{std::make_unique<VimState>()},
                                        hl{std::make_unique<Highlight>()}, runProgram{true}, isCPlusPlus{isCPlusPlus}
{
  cursor = std::make_unique<Cursor>(file.get());
  command = std::make_unique<ColonCommand>(file->getFilePointer());
  warningString = "";
  addView(std::make_unique<NCursesView>(*this));
  addController(std::make_unique<Keyboard>(vs.get()));

}

bool Vim::isCPlusPlusFile() const {
  return isCPlusPlus;
}

int Vim::getCursorX() const {
  return cursor->getX();
}

int Vim::getCursorY() const {
  return cursor->getY();
}

State Vim::getState() const {
  return vs->getState();
}

const std::string &Vim::getFileName() const {
  return file->getFileName();
}

const std::vector<std::string> &Vim::getFileBuffer() const {
  return file->getBuffer();
}

std::pair<int,int> Vim::deleteMotion(std::pair<int,int> motion) {
  int cursorX = getCursorX();
  int cursorY = getCursorY();
  if (motion.first != -1 && motion.second != -1) {
    if (motion.second > cursorY) {
      if (motion.first == 0 && cursorX != 0) {
        file->remove(std::make_pair(cursorX, cursorY), std::make_pair(getFileBuffer()[motion.second - 1].size(), motion.second - 1));
      }
      else {
        file->remove(std::make_pair(cursorX, cursorY), std::make_pair(motion.first, motion.second));
      }
    }
    else if (motion.second == cursorY) {
      if (motion.first < cursorX) {
        file->remove(std::make_pair(motion.first, motion.second), std::make_pair(cursorX, cursorY));
        cursor->moveCursorLeft(abs(motion.first - cursorX));
      }
      else {
        file->remove(std::make_pair(cursorX, cursorY), std::make_pair(motion.first, motion.second));
      }
    }
    else {
      if (cursorX == 0 && motion.first != 0) {
        file->remove(std::make_pair(motion.first, motion.second), std::make_pair(getFileBuffer()[cursorY - 1].size(), cursorY - 1));
      }
      else {
        file->remove(std::make_pair(motion.first, motion.second), std::make_pair(cursorX, cursorY));
      }
      cursor->moveCursorX(motion.first);
      cursor->moveCursorY(motion.second);
    }
  }
  else if (motion.first != -1) {
    if (motion.first > cursorX) {
      file->removeString(cursorY, cursorX, motion.first - 1);
      if (static_cast<size_t>(cursorX) >= file->getBuffer()[cursorY].size()) {
        cursor->moveCursorX(file->getBuffer()[cursorY].size() - 1);
      }
    }
    else if (motion.first < cursorX) {
      file->removeString(cursorY, motion.first, cursorX - 1);
      cursor->moveCursorX(motion.first);
    }
  }
  else if (motion.second != -1) {
    std::pair<int,int> offSet;
    if (motion.second >= cursorY) {
      offSet = file->removeLines(cursorY, motion.second);
      cursor->moveCursorX(0);
    }
    else if (motion.second < cursorY) {
      offSet = file->removeLines(motion.second, cursorY);
      cursor->moveCursorY(motion.second);
      cursor->moveCursorX(0);
    }

    cursor->moveCursorY(cursor->getY() + offSet.second);
  }
  return std::make_pair(cursorX, cursorY);
}

void Vim::changeMotion(std::pair<int,int> motion) {
  std::pair<int,int> originalCursorLocation = deleteMotion(motion);
  if (motion.second != -1 && motion.first == 0 && originalCursorLocation.first == 0) {
    file->insertNewLine(cursor->getX(), cursor->getY());
  }
  else if (motion.second != -1 && motion.first == -1) {
    file->insertNewLine(cursor->getX(), cursor->getY());
  }
}

void Vim::copyMotion(std::pair<int,int> motion) {
  int cursorX = getCursorX();
  int cursorY = getCursorY();
  if (motion.first != -1 && motion.second != -1) {
    file->setClipboardVector(std::make_pair(getCursorX(), getCursorY()), motion);
  }
  else if (motion.first != -1) {
    if (motion.first < cursorX) {
      file->setClipboardString(cursorY, motion.first, cursorX - 1);
    }
    else if(motion.first > cursorX) {
      file->setClipboardString(cursorY, cursorX, motion.first - 1);
    }
  }
  else if (motion.second != -1) {
    if (motion.second >= cursorY) {
      file->setClipboardLines(cursorY, motion.second);
    }
    else if (motion.second < cursorY) {
      file->setClipboardLines(motion.second, cursorY);
      for (int j = motion.second; j < cursorY; ++j) {
        cursor->moveCursorUp();
      }
    }
  }
}

std::pair<int,int> Vim::getMotion(Action currentAction) {
  State currentState = vs->getState();
  int originalMultiplier = max(vs->getMultiplier(), 1);
  int secondMultiplier = 1;
  vs->setMultiplier(0);
  int cursorX = getCursorX();
  int cursorY = getCursorY();
  while(true) {
    std::pair<int,int> actionPair;

    if(sq.getActionSequence().size() == 0) {
      updateViews();
      displayViews();
      actionPair = getAction();
    } else {
      actionPair = sq.getSequenceItem();
    }

    secondMultiplier = max(secondMultiplier, vs->getMultiplier());
    int shift = originalMultiplier * secondMultiplier;
    if (Action::MULT == actionPair.first) {
      sq.addAction(actionPair);
      int tmp = vs->getMultiplier();
      vs->setMultiplier(tmp * 10 + actionPair.second - 48);
      vs->setState(State::MULT_MODE);
      continue;
    }
    else {
      vs->setState(currentState);
    }
    if (Action::CURSOR_LEFT == actionPair.first) {
      sq.addAction(actionPair);
      return std::make_pair(max(0, cursorX - shift), -1);
    }
    else if (Action::CURSOR_DOWN == actionPair.first) {
      sq.addAction(actionPair);
      int fileLength = file->getBuffer().size();
      return std::make_pair(-1, min(fileLength - 1, cursorY + shift));
    }
    else if (Action::CURSOR_UP == actionPair.first) {
      sq.addAction(actionPair);
      return std::make_pair(-1, max(0, cursorY - shift));
    }
    else if (Action::CURSOR_RIGHT == actionPair.first) {
      sq.addAction(actionPair);
      int lineLength = file->getBuffer()[cursorY].size();
      return std::make_pair(min(lineLength, cursorX + shift), -1);
    }
    else if (Action::FIND == actionPair.first) {
      sq.addAction(actionPair);
      vs->setState(State::FIND_MODE);
      std::pair<int,int> result = getWord(true, shift, true);
      if (result.first == cursorX && result.second == cursorY) {
        return std::make_pair(-1, -1);
      }
      return result;
    }
    else if (Action::RFIND == actionPair.first) {
      sq.addAction(actionPair);
      vs->setState(State::RFIND_MODE);
      std::pair<int,int> result = getWord(false, shift, true);
      if (result.first == cursorX && result.second == cursorY) {
        return std::make_pair(-1, -1);
      }
      return result;
    }
    else if (Action::MOVE_TO_FRONT == actionPair.first) {
      sq.addAction(actionPair);
      return std::make_pair(0, -1);
    }
    else if (Action::MOVE_TO_END == actionPair.first) {
      sq.addAction(actionPair);
      return std::make_pair(max(0, static_cast<int>(getFileBuffer()[getCursorY()].size())), -1);
    }
    else if (Action::MOVE_TO_NONBLANK == actionPair.first) {
      sq.addAction(actionPair);
      std::pair<int,int> locPair = file->moveToNonblank(getCursorY());
      if (locPair.first != -1 && locPair.second != -1) {
        return std::make_pair(locPair.first, -1);
      }
      return std::make_pair(-1, -1);
    }
    else if (Action::EXECUTE_FINDCHAR == actionPair.first) {
      sq.addAction(actionPair);
      std::pair<int,int> locPair = file->findRecentlyUsedChar(getCursorX(), getCursorY(), shift);
      if (locPair.first != -1 && locPair.second != -1) {
        if (file->getCharSearchDirection()) {
          return std::make_pair(locPair.first + 1, -1);
        }
        else {
          return std::make_pair(locPair.first, -1);
        }
      }
      return std::make_pair(-1, -1);
    }
    else if (Action::FIND_CHARACTER == actionPair.first) {
      sq.addAction(actionPair);
      State temp = vs->getState();
      vs->setState(State::FIND_CHARACTER_MODE);
      std::pair<int,int> locPair = getCharacter(true, shift, true);
      vs->setState(temp);
      if (getCursorX() == locPair.first && getCursorY() == locPair.second) {
        return std::make_pair(-1, -1);
      }
      else {
        return std::make_pair(locPair.first + 1, -1);
      }
    }
    else if (Action::RFIND_CHARACTER == actionPair.first) {
      sq.addAction(actionPair);
      State temp = vs->getState();
      vs->setState(State::FIND_CHARACTER_MODE);
      std::pair<int,int> locPair = getCharacter(false, shift, true);
      vs->setState(temp);
      if (getCursorX() == locPair.first && getCursorY() == locPair.second) {
        return std::make_pair(-1, -1);
      }
      else {
        return std::make_pair(locPair.first, -1);
      }
    }
    else if (Action::FIND_NEXT == actionPair.first) {
      sq.addAction(actionPair);
      std::pair<int,int> locPair = file->findNext(getCursorX(), getCursorY(), shift);
      if (locPair.first != -1 && locPair.second != -1) {
        return locPair;
      }
      else {
        return std::make_pair(-1, -1);
      }
    }
    else if (Action::FIND_PREV == actionPair.first) {
      sq.addAction(actionPair);
      std::pair<int,int> locPair = file->findPrev(getCursorX(), getCursorY(), shift);
      if (locPair.first != -1 && locPair.second != -1) {
        return locPair;
      }
      else {
        return std::make_pair(-1, -1);
      }
    }
    else if (currentAction == actionPair.first) {
      sq.addAction(actionPair);
      int fileLength = file->getBuffer().size();
      return std::make_pair(-1, min(fileLength - 1, cursorY + shift - 1));
    }
    else {
      sq.clearCurrentActions();
      return std::make_pair(-1, -1);
    }
  }
}

std::pair<int,int> Vim::getWord(bool forward, int multiplier, bool record) {
  std::string currentWord;
  if (forward) {
    currentWord = "/";
  }
  else {
    currentWord = "?";
  }
  int originalCursorX = cursor->getX();
  int originalCursorY = cursor->getY();
  std::pair<int,int> result = std::make_pair(-1, -1);
  while (true) {
    searchString = currentWord;
    std::pair<int,int> actionPair;

    if(sq.getActionSequence().size() == 0) {
      updateViews();
      displayViews();
      actionPair = getAction();
    }
    else {
      actionPair = sq.getSequenceItem();
    }

    if (Action::CHARACTER == actionPair.first) {
      if (record) {
        sq.addAction(actionPair);
      }
      currentWord += static_cast<char>(actionPair.second);
    }
    else if (Action::BACKSPACE == actionPair.first && currentWord.size() >= 1) {
      if (record) {
        sq.addAction(actionPair);
      }
      currentWord = currentWord.erase(currentWord.size() - 1);
      if (currentWord == "") {
        break;
      }
    }
    else if (Action::NEWLINE == actionPair.first) {
      if (record) {
        sq.addAction(actionPair);
      }
      break;
    }
    else if (Action::ESCAPE == actionPair.first) {
      if (record) {
        sq.clearCurrentActions();
      }
      currentWord = "";
      break;
    }
    else if (Action::TAB == actionPair.first) {
      if (record) {
        sq.addAction(actionPair);
      }
      currentWord += '\t';
    }
    result = std::make_pair(-1, -1);
    if (forward && currentWord.size() >= 2) {
      result = file->findInBuffer(currentWord.substr(1), originalCursorX, originalCursorY, multiplier, true);
    }
    else if (!forward && currentWord.size() >= 2) {
      result = file->findInBuffer(currentWord.substr(1), originalCursorX, originalCursorY, multiplier, false);
    }
    if (result.first != -1 && result.second != -1) {
      hl->setHighlight(result.first, result.first + currentWord.size() - 2, result.second);
    }
    else {
      hl->setHighlight(-1, -1, -1);
    }
  }
  searchString = "";
  hl->setHighlight(-1, -1, -1);
  if (currentWord.size() > 1 && result.first != -1 && result.second != -1) {
    file->setRecentSearch(currentWord.substr(1));
    return std::make_pair(result.first, result.second);
  }
  else {
    return std::make_pair(originalCursorX, originalCursorY);
  }
}

std::pair<int,int> Vim::findWord(bool forwardDirection, int multiplier, bool record, bool setCursor) {
  std::pair<int,int> newCursorPosition = getWord(forwardDirection, multiplier, record);
  if (setCursor) {
    cursor->moveCursorX(newCursorPosition.first);
    cursor->moveCursorY(newCursorPosition.second);
  }
  vs->setState(State::NORMAL_MODE);
  return newCursorPosition;
}

std::pair<int,int> Vim::getCharacter(bool forward, int multiplier, bool record) {
  std::pair<int,int> actionPair;
  char c;
  std::pair<int,int> location;
  if(sq.getActionSequence().size() == 0) {
    updateViews();
    displayViews();
    actionPair = getAction();
  }
  else {
    actionPair = sq.getSequenceItem();
  }

  if (Action::CHARACTER == actionPair.first) {
    if (record) {
      sq.addAction(actionPair);
    }
    c =  static_cast<char>(actionPair.second);
  }
  else if (Action::TAB == actionPair.first) {
    if (record) {
      sq.addAction(actionPair);
    }
    c = '\t';
  }
  else {
    if (record) {
      sq.clearCurrentActions();
    }
  }
  if (forward) {
    location = file->findChar(getCursorX(), getCursorY(), multiplier, c);
  }
  else {
    location = file->rfindChar(getCursorX(), getCursorY(), multiplier, c);
  }
  if (location.first != -1 && location.second != -1) {
    return location;
  }
  else {
    return std::make_pair(getCursorX(), getCursorY());
  }
}

std::pair<int,int> Vim::findCharacter(bool forwardDirection, int multiplier, bool record, bool setCursor) {
  std::pair<int,int> newCursorPosition = getCharacter(forwardDirection, multiplier, record);
  if (setCursor) {
    cursor->moveCursorX(newCursorPosition.first);
    cursor->moveCursorY(newCursorPosition.second);
  }
  vs->setState(State::NORMAL_MODE);
  return newCursorPosition;
}

void Vim::replaceMode(bool singleTime, int multiplier) {
  if (!singleTime) {
    file->createBackup();
  }
  while (true) {
    std::pair<int,int> actionPair;
    if(sq.getActionSequence().size() == 0) {
      updateViews();
      displayViews();
      actionPair = getAction();
    }
    else {
      actionPair = sq.getSequenceItem();
    }

    if (Action::CHARACTER == actionPair.first) {
      sq.addAction(actionPair);
      bool success = file->replaceString(static_cast<char>(actionPair.second), getCursorX(), getCursorY(), singleTime, multiplier);
      if (!singleTime) {
        cursor->moveCursorX(cursor->getX() + 1);
      }
      else if (success){
        cursor->moveCursorRight(multiplier - 1);
      }
    }
    else if (Action::NEWLINE == actionPair.first) {
      sq.addAction(actionPair);
      bool success = file->replaceNewLine(getCursorX(), getCursorY(), singleTime, multiplier);
      if (success) {
        cursor->moveCursorX(0);
        cursor->moveCursorDown();
      }
    }
    else if (Action::TAB == actionPair.first) {
      sq.addAction(actionPair);
      bool success = file->replaceString('\t', getCursorX(), getCursorY(), singleTime, multiplier);
      if (!singleTime) {
        cursor->moveCursorX(cursor->getX() + 1);
      }
      else if (success){
        cursor->moveCursorRight(multiplier - 1);
      }
    }
    else {
      if (singleTime) {
        sq.clearCurrentActions();
      }
      else {
        if (Action::BACKSPACE == actionPair.first) {
          sq.addAction(actionPair);
          std::pair<int,int> loc = file->replaceBackward(getCursorX(), getCursorY());
          cursor->moveCursorX(loc.first);
          cursor->moveCursorY(loc.second);
        }
        else if (Action::ESCAPE == actionPair.first) {
          cursor->moveCursorLeft();
          sq.addAction(actionPair);
          break;
        }
      }
    }
    if (singleTime) {
      break;
    }
  }
  if (!singleTime) {
    file->clearBackup();
  }
  file->updateHistory();
  vs->setState(State::NORMAL_MODE);
}

void Vim::commandMode() {
  bool validAction = true;
  searchString = command->getString();
  updateViews();
  displayViews();
  while (true) {
    std::pair<int,int> actionPair;
    if(sq.getActionSequence().size() == 0) {
      updateViews();
      displayViews();
      actionPair = getAction();
    }
    else {
      actionPair = sq.getSequenceItem();
    }

    if (Action::CHARACTER == actionPair.first) {
      command->append(static_cast<char>(actionPair.second));
    }
    else if (Action::NEWLINE == actionPair.first) {
      break;
    }
    else if (Action::TAB == actionPair.first) {
      command->append('\t');
    }
    else if (Action::BACKSPACE == actionPair.first) {
      command->remove();
      if (command->getString() == "") {
        validAction = false;
        break;
      }
    }
    else if (Action::ESCAPE == actionPair.first) {
      validAction = false;
      break;
    }
    searchString = command->getString();
  }
  searchString = "";
  if (validAction) {
    if (command->executeFileCommand(getFileBuffer())) {
      if (command->quitFile()) {
        runProgram = false;
      }
      else if (command->getWarning() != "") {
        warningString = command->getWarning();
      }
    }
    else {
      if (command->getString() == ":r") {
        file->insertStringVector(getCursorX(), getCursorY(), command->getFileVector());
        cursor->moveCursorDown();
        std::pair<int,int> location = file->moveToNonblank(getCursorY());
        if (location.first != -1 && location.second != -1) {
          cursor->moveCursorX(location.first);
        }
        file->updateHistory();
      }
      else if (command->getString() == ":$") {
        std::pair<int,int> loc = file->jumpToEnd();
        cursor->moveCursorX(loc.first);
        cursor->moveCursorY(loc.second);
      }
      else if (command->isDigitCommand() != -1){
        std::pair<int,int> loc = file->jumpToLine(command->isDigitCommand());
        cursor->moveCursorX(loc.first);
        cursor->moveCursorY(loc.second);
      }
    }
  }
  vs->setState(State::NORMAL_MODE);
  command->reset();
}

const Highlight &Vim::getHighlight() const {
  return *(hl.get());
}

const std::string &Vim::getSearchString() const {
  return searchString;
}

std::string &Vim::getWarningString() {
  return warningString;
}

void Vim::run() {
  std::pair<int, int> actionPair;
  while(runProgram) {

    if(sq.getActionSequence().size() == 0) {
      updateViews();
      displayViews();
      actionPair = getAction();
    } else {
      actionPair = sq.getSequenceItem();
    }

    int multiplier = max(1, vs->getMultiplier());

    if (getState() == State::NORMAL_MODE || getState() == State::MULT_MODE) {

      if(getState() == State::NORMAL_MODE) {
        sq.updateActions();
      }

      if (Action::MULT != actionPair.first && getState() == State::MULT_MODE) {
        vs->setState(State::NORMAL_MODE);
      }

      if (Action::MULT == actionPair.first) {
        sq.addAction(actionPair);
        int tmp = vs->getMultiplier();
        vs->setMultiplier(tmp * 10 + actionPair.second - 48);
        vs->setState(State::MULT_MODE);
      }
      else if (Action::INSERT == actionPair.first) {
        sq.addAction(actionPair);
        vs->setState(State::INSERT_MODE);
      }
      else if (Action::INSERT_AT_BEGINNING == actionPair.first) {
        sq.addAction(actionPair);
        std::pair<int,int> location = file->moveToNonblank(getCursorY());
        if (location.first != -1 && location.second != -1) {
          cursor->moveCursorX(location.first);
        }
        vs->setState(State::INSERT_MODE);
      }
      else if(Action::COPY == actionPair.first) {
        vs->setState(State::COPY_MODE);
      }
      else if(Action::CHANGE == actionPair.first) {
        sq.addAction(actionPair);
        vs->setState(State::CHANGE_MODE);
      }
      else if(Action::DEL == actionPair.first) {
        sq.addAction(actionPair);
        vs->setState(State::DEL_MODE);
      }
      else if (Action::FIND == actionPair.first) {
        vs->setState(State::FIND_MODE);
      }
      else if (Action::RFIND == actionPair.first) {
        vs->setState(State::RFIND_MODE);
      }
      else if (Action::FIND_NEXT == actionPair.first) {
        std::pair<int,int> location = file->findNext(getCursorX(), getCursorY(), multiplier);
        if (location.first != -1 && location.second != -1) {
          cursor->moveCursorX(location.first);
          cursor->moveCursorY(location.second);
        }
      }
      else if (Action::FIND_PREV == actionPair.first) {
        std::pair<int,int> location = file->findPrev(getCursorX(), getCursorY(), multiplier);
        if (location.first != -1 && location.second != -1) {
          cursor->moveCursorX(location.first);
          cursor->moveCursorY(location.second);
        }
      }
      else if (Action::FIND_CHARACTER == actionPair.first) {
        vs->setState(State::FIND_CHARACTER_MODE);
      }
      else if (Action::RFIND_CHARACTER == actionPair.first) {
        vs->setState(State::RFIND_CHARACTER_MODE);
      }
      else if (Action::EXECUTE_FINDCHAR == actionPair.first) {
        std::pair<int,int> location = file->findRecentlyUsedChar(getCursorX(), getCursorY(), multiplier);
        if (location.first != -1 && location.second != -1) {
          cursor->moveCursorX(location.first);
          cursor->moveCursorY(location.second);
        }
      }
      else if(Action::PASTE_BEFORE == actionPair.first) {
        sq.addAction(actionPair);
        file->paste(cursor->getX(), cursor->getY(), false);
        std::vector<std::string> tmp = file->getClipboard();
        if (tmp.size() == 1) {
          for (size_t j = 0; j < tmp[0].size() - 1; ++j) {
            cursor->moveCursorRight();
          }
        }
        else if (tmp.size() >= 2) {
          cursor->moveCursorX(0);
        }

        file->updateHistory();
      }
      else if(Action::PASTE_AFTER == actionPair.first) {
        sq.addAction(actionPair);
        file->paste(cursor->getX(), cursor->getY(), true);
        std::vector<std::string> tmp = file->getClipboard();
        if (tmp.size() == 1) {
          for (size_t j = 0; j < tmp[0].size(); ++j) {
            cursor->moveCursorRight();
          }
        }
        else if (tmp.size() >= 2) {
          cursor->moveCursorDown();
          cursor->moveCursorX(0);
        }

        file->updateHistory();
      }
      else if (Action::DEL_KEY == actionPair.first) {
        sq.addAction(actionPair);
        for (int j = 0; j < multiplier; ++j) {
          file->removeForward(getCursorX(), getCursorY(), true);
        }
        file->updateHistory();
        cursor->adjust();
      }
      else if (Action::BACKSPACE == actionPair.first) {
        sq.addAction(actionPair);
        for (int j = 0; j < multiplier; ++j) {
          file->removeBackward(getCursorX(), getCursorY(), true);
          cursor->moveCursorLeft();
        }
        file->updateHistory();
      }
      else if(Action::CURSOR_LEFT == actionPair.first) {
        cursor->moveCursorLeft(multiplier);
      }
      else if(Action::CURSOR_DOWN == actionPair.first) {
        cursor->moveCursorDown(multiplier);
      }
      else if (Action::CURSOR_UP == actionPair.first) {
        cursor->moveCursorUp(multiplier);
      }
      else if (Action::CURSOR_RIGHT == actionPair.first) {
        cursor->moveCursorRight(multiplier);
      }
      else if (Action::MOVE_TO_END == actionPair.first) {
        cursor->moveCursorX(max(0, static_cast<int>(getFileBuffer()[getCursorY()].size()) - 1));
      }
      else if (Action::MOVE_TO_FRONT == actionPair.first) {
        cursor->moveCursorX(0);
      }
      else if (Action::MOVE_TO_NONBLANK == actionPair.first) {
        std::pair<int,int> location = file->moveToNonblank(getCursorY());
        if (location.first != -1 && location.second != -1) {
          cursor->moveCursorX(location.first);
        }
      }
      else if (Action::UNDO == actionPair.first) {
        for(int i = 0; i < multiplier; ++i) {
          std::pair<int,int> newPosition = file->undo();
          if(newPosition.first != -1) {
            cursor->moveCursorX(newPosition.first);
            cursor->moveCursorY(newPosition.second);
          }
        }
        cursor->adjust();
      }
      else if (Action::LAST_ACTION == actionPair.first) {
        sq.updateActionSequence();
      }
      else if (Action::END_OF_LINE_INSERT == actionPair.first) {
        sq.addAction(actionPair);
        cursor->moveCursorX(getFileBuffer()[getCursorY()].size());
        vs->setState(State::INSERT_MODE);
      }
      else if (Action::NEXT_CHARACTER_INSERT == actionPair.first) {
        sq.addAction(actionPair);
        cursor->moveCursorX(getCursorX() + 1);
        vs->setState(State::INSERT_MODE);
      }
      else if (Action::INSERT_LINE_BELOW == actionPair.first) {
        sq.addAction(actionPair);
        file->insertNewLine(getCursorX(), getCursorY(), true);
        cursor->moveCursorDown();
        cursor->moveCursorX(0);
        vs->setState(State::INSERT_MODE);
      }
      else if (Action::INSERT_LINE_ABOVE == actionPair.first) {
        sq.addAction(actionPair);
        file->insertNewLineAbove(getCursorX(), getCursorY());
        cursor->moveCursorX(0);
        vs->setState(State::INSERT_MODE);
      }
      else if (Action::REPLACE_CHARACTER == actionPair.first) { //TO DO
        sq.addAction(actionPair);
        vs->setState(State::REPLACE_CHARACTER_MODE);
      }
      else if (Action::REPLACE == actionPair.first) { //TO DO
        sq.addAction(actionPair);
        vs->setState(State::REPLACE_MODE);
      }
      else if (Action::REMOVE_CHARACTER == actionPair.first) {
        sq.addAction(actionPair);
        deleteMotion(std::make_pair(min(getCursorX() + multiplier, static_cast<int>(getFileBuffer()[getCursorY()].size())), getCursorY()));
        vs->setState(State::INSERT_MODE);
      }
      else if (Action::REMOVE_LINE == actionPair.first) {
        sq.addAction(actionPair);
        deleteMotion(std::make_pair(-1, getCursorY() + multiplier - 1));
        file->insertNewLine(cursor->getX(), cursor->getY());
        vs->setState(State::INSERT_MODE);
      }
      else if (Action::NEXT_WORD == actionPair.first) {
        std::pair<int,int> location = file->moveForwardByWords(getCursorX(), getCursorY(), multiplier);
        cursor->moveCursorX(location.first);
        cursor->moveCursorY(location.second);
      }
      else if (Action::PREV_WORD == actionPair.first) {
        std::pair<int,int> location = file->moveBackwardByWords(getCursorX(), getCursorY(), multiplier);
        cursor->moveCursorX(location.first);
        cursor->moveCursorY(location.second);
      }
      else if (Action::JOIN == actionPair.first) {
        sq.addAction(actionPair);
        std::pair<int,int> location = file->joinLines(getCursorX(), getCursorY(), max(2, multiplier));
        cursor->moveCursorX(location.first);
        file->updateHistory();
      }
      else if (Action::COMMAND == actionPair.first) {
        vs->setState(State::COMMAND_MODE);
      }
      else if (Action::PAGE_BACK == actionPair.first) {
        cursor->moveCursorUp(getViewSize().second);
      }
      else if (Action::PAGE_FORWARD == actionPair.first) {
        cursor->moveCursorDown(getViewSize().second);
      }
      else if (Action::HALF_PAGE_BACK == actionPair.first) {
        cursor->moveCursorUp(getViewSize().second / 2);
      }
      else if (Action::HALF_PAGE_FORWARD == actionPair.first) {
        cursor->moveCursorDown(getViewSize().second / 2);
      }
      else if (Action::DISPLAY_STATS == actionPair.first) {
        warningString = "\""+ file->getFilePointer()->getFileName() + "\"" + " --" + std::to_string(file->getPercentage(getCursorY())) + "%%--";
      }
    }
    else if (getState() == State::INSERT_MODE) {
      if (Action::CHARACTER == actionPair.first) {
        sq.addAction(actionPair);
        file->insertString(static_cast<char>(actionPair.second), cursor->getX(), cursor->getY());
        cursor->moveCursorX(cursor->getX() + 1);
      }
      else if (Action::DEL_KEY == actionPair.first) {
        sq.addAction(actionPair);
        file->removeForward(cursor->getX(), cursor->getY());
      }
      else if (Action::BACKSPACE == actionPair.first) {
        sq.addAction(actionPair);
        std::pair<int,int> location = file->removeBackward(cursor->getX(), cursor->getY());
        cursor->moveCursorY(location.second);
        cursor->moveCursorX(location.first);
      }
      else if (Action::TAB == actionPair.first) {
        sq.addAction(actionPair);
        file->insertString('\t', cursor->getX(), cursor->getY());
        cursor->moveCursorX(cursor->getX() + 1);
      }
      else if (Action::NEWLINE == actionPair.first) {
        sq.addAction(actionPair);
        file->insertNewLine(cursor->getX(), cursor->getY());
        cursor->moveCursorY(cursor->getY() + 1);
        cursor->moveCursorX(0);
      }
      else if (Action::ESCAPE == actionPair.first) {
        sq.addAction(actionPair);
        sq.updateActions();
        vs->setState(State::NORMAL_MODE);
        file->updateHistory();
        cursor->moveCursorLeft();
      }
    }

    if (getState() == State::DEL_MODE) {
      std::pair<int,int> motion = getMotion(Action::DEL);
      deleteMotion(motion);
      vs->setState(State::NORMAL_MODE);
      file->updateHistory();
    }
    else if (getState() == State::CHANGE_MODE) {
      std::pair<int,int> motion = getMotion(Action::CHANGE);
      changeMotion(motion);
      sq.addAction(std::make_pair(Action::INSERT, 0));
      vs->setState(State::INSERT_MODE);
    }
    else if (getState() == State::COPY_MODE) {
      std::pair<int,int> motion = getMotion(Action::COPY);
      copyMotion(motion);
      vs->setState(State::NORMAL_MODE);
    }
    else if (getState() == State::FIND_MODE) {
      findWord(true, max(1, vs->getMultiplier()));
    }
    else if (getState() == State::RFIND_MODE) {
      findWord(false, max(1, vs->getMultiplier()));
    }
    else if (getState() == State::FIND_CHARACTER_MODE) {
      findCharacter(true, max(1, vs->getMultiplier()));
    }
    else if (getState() == State::RFIND_CHARACTER_MODE) {
      findCharacter(false, max(1, vs->getMultiplier()));
    }
    else if (getState() == State::REPLACE_MODE) {
      replaceMode(false);
    }
    else if (getState() == State::REPLACE_CHARACTER_MODE) {
      replaceMode(true, max(1, vs->getMultiplier()));
    }
    else if (getState() == State::COMMAND_MODE) {
      commandMode();
    }
    if (Action::MULT != actionPair.first) {
      vs->setMultiplier(0);
    }

  }
}
