#ifndef VIEW_H
#define VIEW_H

#include <utility>

class View{
 public:
  virtual ~View() = default;
  virtual void update() = 0;
  virtual void displayView() = 0;
  virtual void updateStatusBar() = 0;
  virtual std::pair<int,int> windowSize() = 0;
};

#endif
