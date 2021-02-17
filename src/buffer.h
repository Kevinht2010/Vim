#ifndef BUFFER_H
#define BUFFER_H

#include <memory>
#include <utility>
#include "file.h"
#include "action.h"
#include "history.h"
#include "backup.h"

class Buffer {
  std::vector<std::string> buffer;
  std::unique_ptr<File> file;
  std::vector<std::string> clipboard; //if size is 1, contains string, if size >= 2, contains multiple lines
  std::string recentSearch;
  char lastSearchCharacter;

  History history;
  std::unique_ptr<Backup> backup;

  bool searchDirection;
  bool charSearchDirection;
  std::pair<int,int> forwardSearch(int posX, int posY);
  std::pair<int,int> backwardSearch(int posX, int posY);
  int findMatch(const std::string &s, int lineNumber, int startIdx, int &occurence, bool after);
  void setSearchDirection(bool direction);

  std::pair<int,int> moveBackOneWord(int posX, int posY);
  std::pair<int,int> moveForwardOneWord(int posX, int posY);
 public:
  Buffer();
  Buffer(const std::string &fileName);
  const std::vector<std::string> &getBuffer() const;
  const std::string &getFileName() const;
  File *getFilePointer();
  int getPercentage(int posY) const;
  void insertString(char c, int posX, int posY);
  void removeForward(int posX, int posY, bool characterFlag = false); //del key
  std::pair<int,int> removeBackward(int posX, int posY, bool characterFlag = false); //backspace
  void insertNewLine(int posX, int posY, bool jump = false); //new line
  void insertNewLineAbove(int posX, int posY);
  void insertStringVector(int posX, int posY, const std::vector<std::string> &stringVector);

  void removeString(int posY, int l, int r);
  std::pair<int,int> removeLines(int start, int end);

  bool replaceString(char c, int posX, int posY, bool singleTime, int mult = 1);
  bool replaceNewLine(int posX, int posY, bool singleTime, int mult = 1);
  std::pair<int,int> replaceBackward(int posX, int posY);
  void createBackup();
  void clearBackup();

  void setClipboardString(int posY, int l, int r);
  void setClipboardLines(int start, int end);
  void setClipboardVector(std::pair<int,int> start, std::pair<int,int> end);

  void paste(int posX, int posY, bool after);

  const std::vector<std::string> &getClipboard() const;

  void setRecentSearch(const std::string &s);
  bool getCharSearchDirection() const;
  bool getSearchDirection() const;

  void updateHistory();
  void remove(std::pair<int,int> pos1, std::pair<int,int> pos2);
  void undoRemove(std::pair<int,int> pos1, std::pair<int,int> pos2);
  std::pair<int,int> undo();

  std::pair<int,int> findInBuffer(const std::string &s, int posX, int posY, int mult, bool after, bool setSearch = true);
  std::pair<int,int> findNext(int posX, int posY, int mult);
  std::pair<int,int> findPrev(int posX, int posY, int mult);
  std::pair<int,int> findChar(int posX, int posY, int mult, char c = '\0');
  std::pair<int,int> rfindChar(int posX, int posY, int mult, char c = '\0');
  std::pair<int,int> findRecentlyUsedChar(int posX, int posY, int mult);

  std::pair<int,int> moveBackwardByWords(int posX, int posY, int numWords);
  std::pair<int,int> moveForwardByWords(int posX, int posY, int numWords);
  std::pair<int,int> moveToNonblank(int posY);

  std::pair<int,int> joinLines(int posX, int posY, int mult);

  std::pair<int,int> jumpToEnd();

  std::pair<int,int> jumpToLine(int lineNumber);

  ~Buffer() = default;
};

#endif
