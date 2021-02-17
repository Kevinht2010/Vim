#ifndef HISTORY_H
#define HISTORY_H

#include <vector>
#include <memory>
#include <utility>
#include "history-item.h"
#include "action.h"

class History {
    std::vector<std::vector<HistoryItem>> history; // undo
    std::vector<HistoryItem> currentActions; // current actions in 

  public:
    std::vector<HistoryItem> getUndo() const;
    void deleteLastUndo();

    void addAction(std::pair<int,int> start, std::pair<int,int> end, Action action);
    void addAction(std::pair<int,int> start, std::pair<int,int> end, Action action, std::pair<int,int> cursor);
    void addAction(std::pair<int,int> start, Action action, std::string s);
    void addAction(std::pair<int,int> start, Action action, std::string s, std::pair<int,int> cursor);
    void updateActions();

    ~History() = default;
};


#endif
