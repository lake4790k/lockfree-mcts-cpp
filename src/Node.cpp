#include <atomic>
#include <vector>
#include <cmath>
#include <cassert>
#include <limits>
#include <iostream>

#include "State.hpp"

class Node {
public:
    const double EXPLORATION_CONSTANT = std::sqrt(2);
    const double NO_EXPLORATION = 0;
        
    Node(Node* parent, uint16_t action, State* state):
        parent(parent),
        action(action),
        state(state),
        untakenActions(state->getAvailableActions()),
        untakenIndex(untakenActions->size() - 1) {

        if (!state) throw std::logic_error("state null!");

        for (int i = 0; i < untakenActions->size(); i++) {
            children.push_back(NULL);
        }
    }

    ~Node() {
        delete state;
        for (auto child : children) {
            delete child;
        }
    }

    void releaseParent() {
        parent = NULL;
    }

    void deleteChildrenExcept(Node* except) {
        bool found = false;
        for (auto child : children) {
            if (child == except) {
                found = true;
                continue;
            }
            delete child;
        }
        if (!found) throw std::logic_error("not found child");
        children.clear();
    }

    Node* findChildFor(uint16_t action) {
        for (auto child : children) {
            if (!child) continue;
            if (child->action == action) return child;
        }
        return NULL;
    }

    uint16_t getAction() {
        return action;
    }

    bool isTerminal() {
        return state->isTerminal();
    }

    Node* expand() {
        if (untakenIndex.load()<0) return NULL;
        int16_t untaken = untakenIndex.fetch_sub(1);
        if (untaken < 0) return NULL;

        uint16_t untakenAction = untakenActions->at(untaken);
        State* actionState = state->takeAction(untakenAction);
        Node* node = new Node(this, untaken, actionState);
        children[untaken] = node;
        return node;
    }

    Node* getParent() {
        return parent;
    }

    uint8_t getPreviousAgent() {
        return state->getPreviousAgent();
    }

    void updateRewards(uint64_t reward) {
        visits++;
        rewards += reward;
    }

    bool isVisited() {
        return visits.load() > 0;
    }

    double getUctValue(double c) {
        int visits1 = visits.load();
        return rewards.load() / visits1 + c * std::sqrt(std::log(parent->visits.load()) / visits1);
    }

    Node* childToExploit() {
        return getBestChild(NO_EXPLORATION);
    }

    Node* childToExplore() {
        return getBestChild(EXPLORATION_CONSTANT);
    }

    Node* getBestChild(double c) {
        Node* best = NULL;
        while (!best) {
            double bestValue = std::numeric_limits<double>::lowest();
            for (Node* child : children) {
                if (!child) continue;
                if (!child->isVisited()) continue;

                double childrenValue = child->getUctValue(c);
                if (childrenValue > bestValue) {
                    best = child;
                    bestValue = childrenValue;
                }
            }
        }
        return best;
    }

    State* getStateCopy() {
        return state->copy();
    }

private:
    Node( const Node& other ) = delete;
    Node& operator=( const Node& ) = delete;

    std::atomic<uint64_t> visits{0};

    std::vector<Node*> children;
    std::vector<uint16_t>* untakenActions;
    std::atomic<int16_t> untakenIndex{0};
    std::atomic<uint64_t> rewards{0};

    Node* parent;

    State* const state;
    const uint16_t action;
};

