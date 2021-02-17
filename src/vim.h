#ifndef VIM_H
#define VIM_H

#include "model.h"
#include "cursor.h"
#include "buffer.h"
#include <string>
#include "vim-state.h"
#include "highlight.h"
#include "sequence.h"
#include "colon-command.h"

class Vim : public Model {
  std::string searchString;
  std::string warningString;
  std::unique_ptr<Buffer> file;
  std::unique_ptr<Cursor> cursor;
  std::unique_ptr<VimState> vs;
  std::unique_ptr<Highlight> hl;
  std::unique_ptr<ColonCommand> command;
  Sequence sq;
  bool runProgram;
  bool isCPlusPlus;
  std::pair<int,int> getMotion(Action currentAction);
  std::pair<int,int> deleteMotion(std::pair<int,int> motion);
  void changeMotion(std::pair<int,int> motion);
  void copyMotion(std::pair<int,int> motion);
  std::pair<int,int> getWord(bool forwardDirection, int multiplier, bool record);
  std::pair<int,int> findWord(bool forwardDirection, int multiplier, bool record = false, bool setCursor = true);

  std::pair<int,int> getCharacter(bool forwardDirection, int multiplier, bool record);
  std::pair<int,int> findCharacter(bool forwardDirection, int multiplier, bool record = false, bool setCursor = true);

  void replaceMode(bool singleTime = false, int multiplier = 1);
  void commandMode();
 public:
  Vim(const std::string &fileName, bool isCPlusPlus = false);
  int getCursorX() const;
  int getCursorY() const;
  State getState() const;
  const std::vector<std::string> &getFileBuffer() const;
  const Highlight &getHighlight() const;
  const std::string &getFileName() const;
  const std::string &getSearchString() const;
  std::string &getWarningString();
  bool isCPlusPlusFile() const;

  void run();
};

#endif
