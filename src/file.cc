#include "file.h"
#include <iostream>
#include <fstream>
#include <assert.h>
File::File(const std::string file): fileName{file} {}

const std::string &File::getFileName() const {
  return fileName;
}

void File::readFile(std::vector<std::string> &buffer) {
  std::ifstream fin{fileName};
  fin >> std::noskipws;
  char c;
  std::string store = "";
  while(fin >> c) {
    if (c == '\n' || c == EOF) {
      buffer.push_back(store);
      store = "";
    }
    else {
      store += c;
    }

  }
}

void File::readFileSpaces(std::vector<std::string> &buffer) {
  std::ifstream fin{fileName};
  fin >> std::noskipws;
  char c;
  std::string store = "";
  while(fin >> c) {
    std::cout << c;
    if (isspace(c) || c == EOF) {
      buffer.push_back(store);
      store = "";
    }
    else {
      store += c;
    }
  }
}

void File::saveFile(const std::vector<std::string> &buffer) {
  std::ofstream fout{fileName};
  for(size_t i = 0; i < buffer.size(); ++i) {
    fout << buffer[i] << "\n";
  }
  fout.close();
}

bool File::checkDiff(const std::vector<std::string> &buffer) {
  bool diff = false;
  int cnt = 0;
  std::ifstream fin{fileName};
  fin >> std::noskipws;
  char c;
  std::string store = "";
  while(fin >> c) {
    if (c == '\n' || c == EOF) {
      if (cnt >= static_cast<int>(buffer.size()) || store != buffer[cnt]) {
        diff = true;
        break;
      }
      store = "";
      ++cnt;
    }
    else {
      store += c;
    }
  }
  if (diff == false && cnt == static_cast<int>(buffer.size())) {
    return false;
  }
  else {
    return true;
  }
}
