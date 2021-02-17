#ifndef NCURSES_VIEW_H
#define NCURSES_VIEW_H
#include "ncurses-display.h"
#include "view.h"
#include "vim.h"
#include "scanner.h"

class NCursesView : public View, public NcursesDisplay {
  Vim *vm;
  int screenStart, screenEnd;
  bool startUp;
  const int TAB_LENGTH = 8;
  const int DISPLACEMENT = 6;
  int getLength(const std::string &s) const;
  void reformatScreen(int coordinate, int bufferSize);
  std::unique_ptr<Scanner> scanFile;
 public:
  NCursesView(Vim &vm);
  ~NCursesView() = default;
  void displayView();
  void update();
  void updateStatusBar();
  std::pair<int,int> windowSize();
};

#endif
