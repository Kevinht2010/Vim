#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <memory>
#include "controller.h"
#include "view.h"
#include <utility>
#include <string>
enum Action;

class Model{
  std::vector<std::unique_ptr<View>> views;
  std::unique_ptr<Controller> control;

 protected:
  std::pair<Action, int> getAction();
  void addView(std::unique_ptr<View> v);
  void addController(std::unique_ptr<Controller> c);
  std::pair<int,int> getViewSize(int viewIdx = 0);

 public:
  Model();
  virtual ~Model() = default;
  void updateViews();
  void displayViews();
};

#endif
