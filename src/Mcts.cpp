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

    void setRoot(uint8_t action, State::Ptr state) {
        if (root) {
            Node::Ptr child = root->findChildFor(action);
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

    State::Ptr takeAction() {
        Node::Ptr actionNode  = root->childToExploit();
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
        Node::Ptr child = selectOrExpand();
        State::Ptr terminalState = simulate(child);
        backPropagate(child, terminalState);
    }

    Node::Ptr selectOrExpand() {
        Node::Ptr node(root);
        while (!node->isTerminal()) {
            while (!node->isExpanded()) {
                Node::Ptr expandedNode = node->expand();
                if (expandedNode)
                    return expandedNode;
            }
            node = node->childToExplore();
        }
        return node;
    }

    State::Ptr simulate(const Node::Ptr& node) {
        // TODO copy constructor
        State::Ptr state(node->getState()->copy());
        while (!state->isTerminal()) {
            auto actions = state->getAvailableActions();
            uint16_t randomIdx = 0; // TODO
            uint16_t action = actions->at(randomIdx);
            state->applyAction(action);
        }

        return state;
    }

    void backPropagate(const Node::Ptr& bottom, const State::Ptr& terminalState) {
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

    Node::Ptr root{NULL};
};

