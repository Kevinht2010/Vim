#include "model.h"
#include "ncurses-view.h"
#include "keyboard.h"

Model::Model() {}

std::pair<Action, int> Model::getAction() {
  return control->getAction();
}

void Model::displayViews() {
  for(auto &view : views) {
    view->displayView();
  }
}

void Model::updateViews() {
  for(auto &view : views) {
    view->update();
    view->updateStatusBar();
  }
}

void Model::addView(std::unique_ptr<View> v) {
  views.emplace_back(std::move(v));
}

void Model::addController(std::unique_ptr<Controller> c) {
  control = std::move(c);
}

std::pair<int,int> Model::getViewSize(int viewIdx) {
  return views[viewIdx]->windowSize();
}
