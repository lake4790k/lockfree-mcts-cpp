#include <atomic>
#include <vector>
#include <cmath>
#include <cassert>
#include <limits>
#include <iostream>

#include "Node.hpp"
#include "State.hpp"

        
Node::Node(Node* parent, uint16_t action, State* state):
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

Node::~Node() {
    delete untakenActions;
    delete state;
    for (auto child : children) {
        delete child;
    }
}

void Node::releaseParent() {
    parent = NULL;
}

void Node::deleteChildrenExcept(Node* except) {
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

Node* Node::findChildFor(uint16_t action) {
    for (auto child : children) {
        if (!child) continue;
        if (child->action == action) return child;
    }
    return NULL;
}

uint16_t Node::getAction() {
    return action;
}

bool Node::isTerminal() {
    return state->isTerminal();
}

Node* Node::expand() {
    if (untakenIndex.load()<0) return NULL;
    int16_t untaken = untakenIndex.fetch_sub(1);
    if (untaken < 0) return NULL;

    uint16_t untakenAction = untakenActions->at(untaken);
    State* actionState = state->takeAction(untakenAction);
    Node* node = new Node(this, untakenAction, actionState);
    children[untaken] = node;
    return node;
}

Node* Node::getParent() {
    return parent;
}

uint8_t Node::getPreviousAgent() {
    return state->getPreviousAgent();
}

void Node::updateRewards(uint8_t reward) {
    visits++;
    rewards += reward;
}

bool Node::isVisited() {
    return visits.load() > 0;
}

double Node::getUctValue(double c) {
    int visits1 = visits.load();
    return (double) rewards.load() / visits1 + c * std::sqrt(std::log(parent->visits.load()) / visits1);
}

Node* Node::childToExploit() {
    return getBestChild(NO_EXPLORATION);
}

Node* Node::childToExplore() {
    return getBestChild(EXPLORATION_CONSTANT);
}

Node* Node::getBestChild(double c) {
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

State* Node::getStateCopy() {
    return state->copy();
}


