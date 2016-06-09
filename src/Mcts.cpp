#include <iostream>
#include <chrono>
#include "State.hpp"
#include "Node.cpp"

class Mcts {
public:
    const uint16_t NO_ACTION = 0;

    Mcts(uint8_t threads, uint64_t timePerActionMillis, uint64_t maxIterations):
        threads(threads), 
        timePerActionMillis(timePerActionMillis), 
        maxIterations(maxIterations) {

    }

    void stop() {
        // TODO
    }

    uint8_t getLastAction() { return lastAction; }
    uint64_t getTotalIteration() { return totalIterations.load(); }

    void setRoot(uint8_t action, State& state) {
        if (root) {
            std::shared_ptr<Node> child = root->findChildFor(action);
            if (child) {
                root = child;
                root->releaseParent();
                return;
            }
        }
        root = std::make_shared<Node>((Node*) NULL, NO_ACTION, state);
    }

    void think() {
        if (threads == 1) {
            doThink();
            return;
        }

        // TODO
    }

    std::shared_ptr<State> takeAction() {
        std::shared_ptr<Node> actionNode  = root->childToExploit();
        lastAction = actionNode->getAction();
        root = actionNode;
        return actionNode->getState();
    }


private:

    void doThink() {
        std::chrono::time_point<std::chrono::system_clock> start;
        start = std::chrono::system_clock::now();
        uint64_t i = 0;
        // TODO time
        while (i++ < maxIterations || !root->isExpanded()) {
            growTree();
            totalIterations++;
        }
    }

    void growTree() {
        std::shared_ptr<Node> child = selectOrExpand();
        std::unique_ptr<State> terminalState = simulate(child);
        backPropagate(child, terminalState);
    }

    std::shared_ptr<Node> selectOrExpand() {
        std::shared_ptr<Node> node(root);
        while (!node->isTerminal()) {
            while (!node->isExpanded()) {
                std::shared_ptr<Node> expandedNode = node->expand();
                if (expandedNode)
                    return expandedNode;
            }
            node = node->childToExplore();
        }
        return node;
    }

    std::unique_ptr<State> simulate(const std::shared_ptr<Node>& node) {
        // TODO copy constructor
        std::unique_ptr<State> state(&node->getState()->copy());
        while (!state->isTerminal()) {
            auto actions = state->getAvailableActions();
            uint16_t randomIdx = 0; // TODO
            uint16_t action = actions->at(randomIdx);
            state->applyAction(action);
        }

        return state;
    }

    void backPropagate(const std::shared_ptr<Node>& bottom, const std::unique_ptr<State> & terminalState) {
        Node* node = bottom.get();
        while (node) {
            double reward = terminalState->getRewardFor(node->getPreviousAgent());
            node->updateRewards(reward);
            node = node->getParent().get();
        }
    }

    std::atomic<uint64_t> totalIterations;

    uint64_t timePerActionMillis;
    uint8_t threads;
    uint64_t maxIterations;

    uint8_t lastAction{0};

    std::shared_ptr<Node> root{NULL};
};


int main() {
  std::cout << "finished\n";
}

