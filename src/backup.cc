#include "backup.h"

Backup::Backup(){}

void Backup::createBackup(const std::vector<std::string> &file) {
  fileBackup = file;
}

void Backup::clearBackup() {
  fileBackup.clear();
}

bool Backup::existsCharacter(int posX, int posY) const {
  if (0 <= posY && posY < static_cast<int>(fileBackup.size()) && posX >= 0 && posX < static_cast<int>(fileBackup[posY].size())) {
    return true;
  }
  return false;
}

char Backup::accessCharacter(int posX, int posY) const {
  return fileBackup[posY][posX];
}

void Backup::insertLine(int posX, int posY) {
  if (posX > static_cast<int>(fileBackup[posY].size())) {
    posX = static_cast<int>(fileBackup[posY].size());
  }
  if (posY < static_cast<int>(fileBackup.size())) {
    std::string tmpString = fileBackup[posY].substr(posX);
    fileBackup.insert(fileBackup.begin() + (posY + 1), tmpString);
  }
}

void Backup::removeLine(int posX, int posY) {
  if (posX == 0 && posY > 0 && posY < static_cast<int>(fileBackup.size())) {
    std::string tmpString = fileBackup[posY];
    fileBackup.erase(fileBackup.begin() + posY);
    fileBackup[posY - 1] += tmpString;
  }
}
