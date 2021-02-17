#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "action.h"
#include <utility>
#include <string>

class Controller {
  virtual std::pair<Action, int> action() = 0;
 public:
  std::pair<Action, int> getAction();
  virtual ~Controller() = default;
};

#endif
