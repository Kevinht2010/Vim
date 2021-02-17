#include "history-item.h"
#include "action.h"
#include <string>

HistoryItem::HistoryItem(std::pair<int,int> start, Action a, std::string s) {
    cursorPosition = start;
    startPosition = start;
    action = Action::INSERT;
    str = s;
}

HistoryItem::HistoryItem(std::pair<int,int> start, Action a, std::string s, std::pair<int,int> cursor) {
    cursorPosition = cursor;
    startPosition = start;
    action = Action::INSERT;
    str = s;
}

HistoryItem::HistoryItem(std::pair<int,int> start, std::pair<int,int> end, Action a) {
    cursorPosition = start;
    startPosition = start;
    endPosition = end;
    action = Action::DEL;
}

HistoryItem::HistoryItem(std::pair<int,int> start, std::pair<int,int> end, Action a, std::pair<int,int> cursor) {
    cursorPosition = cursor;
    startPosition = start;
    endPosition = end;
    action = Action::DEL;
}

Action HistoryItem::getAction() const {
    return action;
}

std::pair<int,int> HistoryItem::getCursorPosition() const {
    return cursorPosition;
}

std::pair<int,int> HistoryItem::getStartPosition() const {
    return startPosition;
}

std::pair<int,int> HistoryItem::getEndPosition() const {
    return endPosition;
}

std::string HistoryItem::getString() const {
    return str;
}
