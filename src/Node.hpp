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
        
    Node(Node* parent, uint16_t action, State* state);

    ~Node();

    void releaseParent();

    void deleteChildrenExcept(Node* except);

    Node* findChildFor(uint16_t action);

    uint16_t getAction();

    bool isTerminal();

    Node* expand();

    Node* getParent();

    uint8_t getPreviousAgent();

    void updateRewards(uint8_t reward);

    bool isVisited();

    double getUctValue(double c);

    Node* childToExploit();

    Node* childToExplore();

    Node* getBestChild(double c);

    State* getStateCopy();

private:
    Node( const Node& other ) = delete;
    Node& operator=( const Node& ) = delete;

    std::atomic<uint64_t> visits{0};
    std::atomic<uint64_t> rewards{0};

    std::vector<Node*> children;
    std::vector<uint16_t>* untakenActions;
    std::atomic<int16_t> untakenIndex{0};

    Node* parent;

    State* const state;
    const uint16_t action;
};

