#include "controller.h"

std::pair<Action, int> Controller::getAction() {
  return action();
}
