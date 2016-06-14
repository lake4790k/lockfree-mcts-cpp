#pragma once

#include <atomic>
#include <chrono>
#include <random>
#include <memory>
#include "Threads.hpp"

class Node;
class State;


class Mcts {
public:
    const uint16_t NO_ACTION = -1;

    typedef std::chrono::high_resolution_clock clock;

    Mcts(std::shared_ptr<Threads> pool, uint8_t threads, uint64_t timePerActionMillis, uint64_t maxIterations);
    ~Mcts();

    uint8_t getLastAction();
    uint64_t getTotalIteration();

    void setRoot(uint8_t action, State* state);

    void think();

    State* takeAction();

private:

    void doThink();

    void growTree(std::mt19937& random);

    Node* selectOrExpand();

    State* simulate(Node* node, std::mt19937& random);

    void backPropagate(Node*, State*);

    std::atomic<uint64_t> totalIterations;

    std::random_device rd;    
    std::mt19937 random;

    uint64_t timePerActionMillis;
    uint8_t threads;
    uint64_t maxIterations;

    uint8_t lastAction{0};

    Node* root{NULL};

    std::shared_ptr<Threads> pool;
};

