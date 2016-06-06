#include <atomic>
#include <vector>
#include <memory>
#include <cmath>
#include "State.hpp"

 
const double EXPLORATION_CONSTANT = std::sqrt(2);
const double NO_EXPLORATION = 0;

class Node {
public:
    Node(Node* parent, uint16_t action, std::shared_ptr<State> state):
        parent(parent),
        action(action),
        state(state),
        untakenActions(state->getAvailableActions()),
        untakenIndex(untakenActions->size() - 1) {

        for (int i = 0; i < untakenActions->size(); i++) {
            children.emplace_back(std::shared_ptr<Node>(NULL));
        }
    }

    ~Node() {}

    std::shared_ptr<Node> findChildFor(uint16_t action) {
        for (auto& child : children) {
            if (!child) continue;
            if (child->action == action) return child;
        }
        return std::shared_ptr<Node>(NULL);
    }

    void releaseParent() {
        parent = NULL;
    }

    uint16_t getAction() {
        return action;
    }

    bool isTerminal() {
        return state->isTerminal();
    }

    bool isExpanded() {
        return untakenIndex.load() < 0;
    }

private:

    std::atomic<uint16_t> untakenIndex;
    std::atomic<uint64_t> visits;

    std::vector<std::shared_ptr<Node>> children;
    std::unique_ptr<std::vector<uint16_t>> untakenActions;
    std::shared_ptr<State> state;
    
    const uint16_t action;

    std::shared_ptr<Node> parent;
    std::atomic<double> rewards;

};
