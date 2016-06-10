#include <atomic>
#include <vector>
#include <memory>
#include <cmath>
#include <cassert>
#include <limits>
#include "State.hpp"

 

class Node {
public:
    typedef std::shared_ptr<Node> Ptr;

    const double EXPLORATION_CONSTANT = std::sqrt(2);
    const double NO_EXPLORATION = 0;
        
    Node(Node* parent, uint16_t action, const State::Ptr& state):
        parent(parent),
        action(action),
        state(state),
        untakenActions(state->getAvailableActions()),
        untakenIndex(untakenActions->size() - 1) {

        for (int i = 0; i < untakenActions->size(); i++) {
            children.emplace_back(Node::Ptr(NULL));
        }
    }

    ~Node() {}

    Node::Ptr findChildFor(uint16_t action) {
        for (auto& child : children) {
            if (!child) continue;
            if (child->action == action) return child;
        }
        return Node::Ptr(NULL);
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

    Node::Ptr expand() {
        uint16_t untaken = untakenIndex.fetch_sub(1);
        if (untaken < 0) return Node::Ptr(NULL);

        uint16_t untakenAction = untakenActions->at(untaken);
        State::Ptr actionState = state->takeAction(untakenAction);
        auto it = children.cbegin();
        it += untaken-1;
        Node::Ptr node = std::make_shared<Node>(this, untaken, actionState);
        children.emplace(it, node);
        return children.at(untaken);
    }

    Node::Ptr getParent() {
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

    Node::Ptr childToExploit() {
        return getBestChild(NO_EXPLORATION);
    }

    Node::Ptr childToExplore() {
        return getBestChild(EXPLORATION_CONSTANT);
    }

    Node::Ptr getBestChild(double c) {
        assert(isExpanded());
        Node::Ptr best;
        for(;;) {
            double bestValue = std::numeric_limits<double>::lowest();
            for (Node::Ptr& child : children) {
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

    State::Ptr getState() {
        return state;
    }

private:

    std::atomic<uint16_t> untakenIndex;
    std::atomic<uint64_t> visits;

    std::vector<Node::Ptr> children;
    std::shared_ptr<std::vector<uint16_t>> untakenActions;
    State::Ptr state;
    
    const uint16_t action;

    Node::Ptr parent;
    std::atomic<uint64_t> rewards;

};
