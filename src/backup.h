#ifndef BACKUP_H
#define BACKUP_H

#include <vector>
#include <string>

class Backup {
  std::vector<std::string> fileBackup;
 public:
  Backup();
  void createBackup(const std::vector<std::string> &file);
  void clearBackup();
  char accessCharacter(int posX, int posY) const;
  bool existsCharacter(int posX, int posY) const;
  void insertLine(int posX, int posY);
  void removeLine(int posX, int posY);
  ~Backup() = default;
};

#endif
