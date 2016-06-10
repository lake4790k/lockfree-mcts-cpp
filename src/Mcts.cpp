#include <random>
#include <chrono>
#include <future>
#include "State.hpp"
#include "Node.cpp"

class Mcts {
public:
    const uint16_t NO_ACTION = 0;

    typedef std::chrono::high_resolution_clock clock;

    Mcts(uint8_t threads, uint64_t timePerActionMillis, uint64_t maxIterations):
        threads(threads), 
        timePerActionMillis(timePerActionMillis), 
        maxIterations(maxIterations) {   }

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

        std::vector<std::future<void>> futures;
        for (int t=0; t<threads; t++) {
            futures.emplace_back(std::async(std::launch::async, &Mcts::doThink, this));
        }
        for (auto& future : futures) {
            future.get();
        }
    }

    State::Ptr takeAction() {
        Node::Ptr actionNode  = root->childToExploit();
        lastAction = actionNode->getAction();
        root = actionNode;
        return actionNode->getState();
    }

private:

    void doThink() {
        std::mt19937 random;
        auto start = clock::now();
        uint64_t i = 0;
        for (;;)  {
            growTree(random);

            totalIterations++;
            auto now = clock::now();
            auto since = now - start;
            auto millisSince = std::chrono::duration_cast<std::chrono::milliseconds>(since);

            if (i++ > maxIterations || millisSince.count() > timePerActionMillis) {
                if (!root->isExpanded()) continue;
                break;
            }
        }
    }

    void growTree(std::mt19937& random) {
        Node::Ptr child = selectOrExpand();
        State::Ptr terminalState = simulate(child, random);
        backPropagate(child, terminalState);
    }

    Node::Ptr selectOrExpand() {
        Node::Ptr node(root);
        while (!node->isTerminal()) {
            while (!node->isExpanded()) {
                Node::Ptr expandedNode = node->expand();
                if (expandedNode) // can be NULL if other thread computes it
                    return expandedNode;
            }
            node = node->childToExplore(); // spins until all childs are computed
        }
        return node;
    }

    State::Ptr simulate(const Node::Ptr& node, std::mt19937& random) {
        // TODO copy constructor
        State::Ptr state(node->getState()->copy());
        while (!state->isTerminal()) {
            auto actions = state->getAvailableActions();
            std::uniform_int_distribution<uint16_t> uniform(0, actions->size());
            uint16_t randomIdx = uniform(random);
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

