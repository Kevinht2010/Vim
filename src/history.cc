#include "history.h"
#include "history-item.h"
#include "action.h"

std::vector<HistoryItem>  History::getUndo() const {
    std::vector<HistoryItem> hist;
    if(history.size() >= 1) {
        hist = history[history.size() - 1];
    }
    return hist;
}

void History::addAction(std::pair<int,int> start, std::pair<int,int> end, Action action) {
    currentActions.push_back(HistoryItem(start, end, action));
}

void History::addAction(std::pair<int,int> start, std::pair<int,int> end, Action action, std::pair<int,int> cursor) {
    currentActions.push_back(HistoryItem(start, end, action, cursor));
}

void History::addAction(std::pair<int,int> start, Action action, std::string s) {
    currentActions.push_back(HistoryItem(start, action, s));
}

void History::addAction(std::pair<int,int> start, Action action, std::string s, std::pair<int,int> cursor) {
    currentActions.push_back(HistoryItem(start, action, s, cursor));
}

void History::updateActions() {
    if(currentActions.size() > 0) {
        history.push_back(currentActions);
        currentActions.clear();
    }
}

void History::deleteLastUndo() {
    history.pop_back();
}
