#ifndef FILE_H
#define FILE_H

#include <vector>
#include <string>

class File {
  std::string fileName;
 public:
  File(const std::string fileName);
  const std::string &getFileName() const;
  void readFile(std::vector<std::string> &buffer);
  void readFileSpaces(std::vector<std::string> &buffer);
  void saveFile(const std::vector<std::string> &buffer);
  bool checkDiff(const std::vector<std::string> &buffer);
  ~File() = default;
};

#endif
