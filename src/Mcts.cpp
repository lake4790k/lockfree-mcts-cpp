#include <future>
#include <iostream>
#include "State.hpp"
#include "Node.hpp"
#include "Mcts.hpp"

Mcts::Mcts(std::shared_ptr<Threads> pool, uint8_t threads, uint64_t timePerActionMillis, uint64_t maxIterations):
    pool(pool),
    threads(threads), 
    timePerActionMillis(timePerActionMillis), 
    maxIterations(maxIterations),
    random(rd()) {   }

Mcts::~Mcts() {
    delete root;
}

uint8_t Mcts::getLastAction() { return lastAction; }

uint64_t Mcts::getTotalIteration() { return totalIterations.load(); }

void Mcts::setRoot(uint8_t action, State* state) {
    if (root) {
        Node* child = root->findChildFor(action);
        if (child) {
            root->deleteChildrenExcept(child);
            delete root;
            root = child;
            root->releaseParent();
            return;     
        } else {
        // throw std::logic_error("could not find root");
            delete root;
        }
    }
    root = new Node(NULL, NO_ACTION, state->copy());
}

void Mcts::think() {
    if (threads == 1) {
        doThink();
        return;
    }

    std::vector<std::future<void>> futures;
    for (int t=0; t<threads; t++) {
        futures.emplace_back(
            pool->invoke([this] {
                this->doThink();
            })
        );
    }
    for (auto& future : futures) {
        future.get();
    }
}

State* Mcts::takeAction() {
    Node* actionNode = root->childToExploit();
    lastAction = actionNode->getAction();
    root->deleteChildrenExcept(actionNode);
    delete root;
    root = actionNode;
    root->releaseParent();
    return actionNode->getStateCopy();
}


void Mcts::doThink() {
    auto start = clock::now();
    uint64_t i = 0;
    for (;;)  {
        growTree(random);
        totalIterations++;
        auto now = clock::now();
        auto since = now - start;
        auto millisSince = std::chrono::duration_cast<std::chrono::milliseconds>(since);

        if (i++ > maxIterations || millisSince.count() > timePerActionMillis) {
            break;
        }
    }
}

void Mcts::growTree(std::mt19937& random) {
    Node* child = selectOrExpand();
    State* terminalState = simulate(child, random);
    backPropagate(child, terminalState);
    delete terminalState;
}

Node* Mcts::selectOrExpand() {
    Node* node = root;
    while (!node->isTerminal()) {
        Node* expandedNode = node->expand();
        if (expandedNode)
            return expandedNode;

        node = node->childToExplore(); // spins until all childs are computed
    }
    return node;
}

State* Mcts::simulate(Node* node, std::mt19937& random) {
    State* state = node->getStateCopy();
    while (!state->isTerminal()) {
        auto actions = state->getAvailableActions();
        std::uniform_int_distribution<uint16_t> uniform(0, actions->size()-1);
        uint16_t randomIdx = uniform(random);
        uint16_t action = actions->at(randomIdx);
        state->applyAction(action);
        delete actions; 
    }
    return state;
}

void Mcts::backPropagate(Node* node, State* terminalState) {
    while (node) {
        uint8_t reward = terminalState->getRewardFor(node->getPreviousAgent());
        node->updateRewards(reward);
        node = node->getParent();
    }
}
