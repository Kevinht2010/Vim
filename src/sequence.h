#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <utility>
#include <vector>
#include "action.h"

class Sequence {
    std::vector<std::pair<int,int>> lastAction;
    std::vector<std::pair<int,int>> currentActions;
    std::vector<std::pair<int,int>> actionSequence;

  public:
    Sequence() = default;
    void clearCurrentActions();
    std::pair<int,int> getSequenceItem();
    void updateActions();
    void updateActionSequence();
    void addAction(std::pair<int,int> actionPair);
    std::vector<std::pair<int,int>> getActionSequence() const;

    ~Sequence() = default;
};

#endif
