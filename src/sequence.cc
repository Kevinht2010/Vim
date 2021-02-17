#include "sequence.h"
#include <utility>
#include "assert.h"

void Sequence::updateActions() {
    if(currentActions.size() > 0) {
        lastAction = currentActions;
        currentActions.clear();
    }
}

void Sequence::updateActionSequence() {
    if(lastAction.size() > 0) {
        actionSequence = lastAction;
    }
}

std::pair<int,int> Sequence::getSequenceItem() {
    std::pair<int,int> actionPair;
    if(actionSequence.size() > 0) {
        actionPair = actionSequence[0];
        actionSequence.erase(actionSequence.begin());
    }

    return actionPair;
}

void Sequence::clearCurrentActions() {
    currentActions.clear();
}

void Sequence::addAction(std::pair<int,int> actionPair) {
    currentActions.push_back(actionPair);
}

std::vector<std::pair<int,int>> Sequence::getActionSequence() const {
    return actionSequence;
}
