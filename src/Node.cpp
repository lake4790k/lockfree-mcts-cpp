#include <atomic>
#include <vector>
#include <memory>
#include <cmath>
#include <cassert>
#include <limits>
#include "State.hpp"

 
const double EXPLORATION_CONSTANT = std::sqrt(2);
const double NO_EXPLORATION = 0;

class Node {
public:
    Node(Node* parent, uint16_t action, State& state):
        parent(parent),
        action(action),
        state(&state),
        untakenActions(state.getAvailableActions()),
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

    std::shared_ptr<Node> expand() {
        uint16_t untaken = untakenIndex.fetch_sub(1);
        if (untaken < 0) return std::shared_ptr<Node>(NULL);

        uint16_t untakenAction = untakenActions->at(untaken);
        State& actionState = state->takeAction(untakenAction);
        auto it = children.cbegin();
        it += untaken-1;
        children.emplace(it, std::make_shared<Node>(this, untaken, actionState));
        return children.at(untaken);
    }

    std::shared_ptr<Node> getParent() {
        return parent;
    }

    uint8_t getPreviousAgent() {
        return state->getPreviousAgent();
    }

    void updateRewards(uint64_t reward) {
        rewards += reward;
    }

    bool isVisited() {
        return visits.load() > 0;
    }

    double getUctValue(double c) {
        int visits1 = visits.load();
        return rewards.load() / visits1 + c * std::sqrt(std::log(parent->visits.load()) / visits1);
    }

    std::shared_ptr<Node> childToExploit() {
        return getBestChild(NO_EXPLORATION);
    }

    std::shared_ptr<Node> childToExplore() {
        return getBestChild(EXPLORATION_CONSTANT);
    }

    std::shared_ptr<Node> getBestChild(double c) {
        assert(isExpanded());
        std::shared_ptr<Node> best;
        for(;;) {
            double bestValue = std::numeric_limits<double>::lowest();
            for (std::shared_ptr<Node>& child : children) {
                while (!child) {}
                while (!child->isVisited()) {}

                double childrenValue = child->getUctValue(c);
                if (childrenValue > bestValue) {
                    best = child;
                    bestValue = childrenValue;
                }
            }
        }
        return best;
    }

    std::shared_ptr<State> getState() {
        return state;
    }

private:

    std::atomic<uint16_t> untakenIndex;
    std::atomic<uint64_t> visits;

    std::vector<std::shared_ptr<Node>> children;
    std::shared_ptr<std::vector<uint16_t>> untakenActions;
    std::shared_ptr<State> state;
    
    const uint16_t action;

    std::shared_ptr<Node> parent;
    std::atomic<uint64_t> rewards;

};
