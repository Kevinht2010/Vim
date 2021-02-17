#include "vim.h"

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    std::string fileName(argv[1]);
    bool isCPlusPlus = false;
    std::size_t found = fileName.find(".");
    if (found != std::string::npos && (fileName.substr(found + 1) == "h" || fileName.substr(found + 1) == "cc")) {
      isCPlusPlus = true;
    }
    Vim v{fileName, isCPlusPlus};
    v.run();
  }
  return 0;
}
