#include <random>
#include <chrono>
#include <future>
#include <iostream>
#include "State.hpp"
#include "Node.cpp"

class Mcts {
public:
    const uint16_t NO_ACTION = -1;

    typedef std::chrono::high_resolution_clock clock;

    Mcts(uint8_t threads, uint64_t timePerActionMillis, uint64_t maxIterations):
        threads(threads), 
        timePerActionMillis(timePerActionMillis), 
        maxIterations(maxIterations) {   }

    ~Mcts() {
        delete root;
    }

    uint8_t getLastAction() { return lastAction; }
    uint64_t getTotalIteration() { return totalIterations.load(); }

    void setRoot(uint8_t action, State* state) {
        if (root) {
            Node* child = root->findChildFor(action);
            if (child) {
                root->deleteChildrenExcept(child);
                delete root;
                root = child;
                root->releaseParent();
                delete state;
                return;     
            } else {
            // throw std::logic_error("could not find root");
                delete root;
            }
        }
        root = new Node(NULL, NO_ACTION, state->copy());
    }

    void think() {
        if (threads == 1) {
            doThink();
            return;
        }

        std::vector<std::future<void>> futures;
        for (int t=0; t<threads; t++) {
            futures.emplace_back(std::async(std::launch::async, &Mcts::doThink, this));
        }
        for (auto& future : futures) {
            future.get();
        }
    }

    State* takeAction() {
        Node* actionNode = root->childToExploit();
        lastAction = actionNode->getAction();
        root->deleteChildrenExcept(actionNode);
        delete root;
        root = actionNode;
        root->releaseParent();
        return actionNode->getStateCopy();
    }

private:

    void doThink() {
        std::random_device rd;    
        std::mt19937 random(rd());
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

    void growTree(std::mt19937& random) {
        Node* child = selectOrExpand();
        double reward = simulate(child, random);
        backPropagate(child, reward);
    }

    Node* selectOrExpand() {
        Node* node = root;
        while (!node->isTerminal()) {
            Node* expandedNode = node->expand();
            if (expandedNode)
                return expandedNode;

            node = node->childToExplore(); // spins until all childs are computed
        }
        return node;
    }

    double simulate(Node* node, std::mt19937& random) {
        State* state = node->getStateCopy();
        while (!state->isTerminal()) {
            auto actions = state->getAvailableActions();
            std::uniform_int_distribution<uint16_t> uniform(0, actions->size()-1);
            uint16_t randomIdx = uniform(random);
            uint16_t action = actions->at(randomIdx);
            state->applyAction(action);
        }
        double reward = state->getRewardFor(node->getPreviousAgent());
        delete state;
        return reward;
    }

    void backPropagate(Node* node, double reward) {
        while (node) {
            node->updateRewards(reward);
            node = node->getParent();
        }
    }

    std::atomic<uint64_t> totalIterations;

    uint64_t timePerActionMillis;
    uint8_t threads;
    uint64_t maxIterations;

    uint8_t lastAction{0};

    Node* root{NULL};
};

