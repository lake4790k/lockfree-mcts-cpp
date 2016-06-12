#pragma once

#include "Mcts.hpp"

class State;

class SelfPlay {
public:

    SelfPlay(
        uint8_t threads1, uint8_t threads2,
        uint64_t timePerActionMillis1, uint64_t timePerActionMillis2,
        uint64_t maxIterations1, uint64_t maxIterations2);


    uint8_t play(State* state);

private:

    std::unique_ptr<Mcts> mcts1;
    std::unique_ptr<Mcts> mcts2;

};

