#ifndef HISTORYITEM_H
#define HISTORYITEM_H

#include <utility>
#include "action.h"
#include <string>

class HistoryItem {
    std::pair<int,int> cursorPosition;
    std::pair<int,int> startPosition;
    std::pair<int,int> endPosition;
    Action action;
    std::string str;

  public:
    HistoryItem();
    HistoryItem(std::pair<int,int> start, Action a, std::string s);
    HistoryItem(std::pair<int,int> start, Action a, std::string s, std::pair<int,int> cursor);
    HistoryItem(std::pair<int,int> start, std::pair<int,int> end, Action a);
    HistoryItem(std::pair<int,int> start, std::pair<int,int> end, Action a, std::pair<int,int> cursor);

    Action getAction() const;
    std::pair<int,int> getCursorPosition() const;
    std::pair<int,int> getStartPosition() const;
    std::pair<int,int> getEndPosition() const;
    std::string getString() const;

    ~HistoryItem() = default;
};

#endif