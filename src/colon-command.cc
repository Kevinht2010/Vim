#include "colon-command.h"

ColonCommand::ColonCommand(File *file) : file{file}, currentCommand{":"}, warning{""}, quit{false}{}

void ColonCommand::append(char c) {
  currentCommand += c;
}

void ColonCommand::remove() {
  currentCommand.erase(currentCommand.begin() + (currentCommand.size() - 1));
}

bool ColonCommand::executeFileCommand(const std::vector<std::string> &buffer) {
  if (currentCommand == ":wq" || currentCommand == ":q!" || currentCommand == ":q" || currentCommand == ":w") {
    if (currentCommand == ":wq") {
      file->saveFile(buffer);
      quit = true;
    }
    else if (currentCommand == ":q!") {
      quit = true;
    }
    else if (currentCommand == ":q") {
      if (file->checkDiff(buffer)) {
        quit = false;
        warning = "No Write Since Last Change";
      }
      else {
        quit = true;
      }
    }
    else if (currentCommand == ":w") {
      file->saveFile(buffer);
      warning = "\"" + file->getFileName() + "\" written";
    }
    return true;
  }
  return false;
}

const std::vector<std::string> &ColonCommand::getFileVector() {
  file->readFile(storeFile);
  return storeFile;
}
int ColonCommand::isDigitCommand() const {
  if (currentCommand.size() >= 2) {
    std::string checkDigits = currentCommand.substr(1);
    bool validNumber = true;
    for (size_t j = 0; j < checkDigits.size(); ++j) {
      if (!isdigit(checkDigits[j])) {
        validNumber = false;
        break;
      }
    }
    if (validNumber) {
      int number = std::stoi(checkDigits);
      return number;
    }
  }
  return -1;
}

void ColonCommand::reset() {
  currentCommand = ":";
  warning = "";
  quit = false;
  storeFile.clear();
}

bool ColonCommand::quitFile() const {
  return quit;
}

std::string ColonCommand::getString() const {
  return currentCommand;
}

std::string ColonCommand::getWarning() const {
  return warning;
}
