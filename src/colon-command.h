#ifndef COLON_COMMAND_H
#define COLON_COMMAND_H

#include <vector>
#include <string>
#include "file.h"

class ColonCommand {
  File *file;
  std::string currentCommand;
  std::string warning;
  bool quit;
  std::vector<std::string> storeFile;
 public:
  ColonCommand(File *file);
  void append(char c);
  void remove();
  bool executeFileCommand(const std::vector<std::string> &buffer);
  const std::vector<std::string> &readFile() const;
  const std::vector<std::string> &getFileVector();
  int isDigitCommand() const;
  bool quitFile() const;
  std::string getWarning() const;
  void reset();
  std::string getString() const;
  ~ColonCommand() = default;
};

#endif
