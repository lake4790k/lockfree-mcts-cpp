#include <iostream>
#include <memory>
#include <random>
#include "Mcts.cpp"

class SelfPlay {
public:

    SelfPlay(
        uint8_t threads1, uint8_t threads2,
        uint64_t timePerActionMillis1, uint64_t timePerActionMillis2,
        uint64_t maxIterations1, uint64_t maxIterations2) {

        mcts1 = std::make_unique<Mcts>(threads1, timePerActionMillis1, maxIterations1);
        mcts2 = std::make_unique<Mcts>(threads2, timePerActionMillis2, maxIterations2);
    }


    uint8_t play(State* state) {
        std::random_device rd;    
        std::mt19937 random(rd());
        std::uniform_int_distribution<uint8_t> uniform01(0,1);
        uint8_t c = uniform01(random);
        uint8_t player = 0;
        uint8_t action = -1;
        while (!state->isTerminal()) {
            player = 1 + c++ %2;
            Mcts* mcts = player == 1
                ? mcts1.get() 
                : mcts2.get();

            mcts->setRoot(action, state);
            mcts->think();

            state = mcts->takeAction();
            action = mcts->getLastAction();
        }
        bool draw = state->getWinner() == 0;
        return !draw
            ? player
            : 0;
    }


private:

    std::unique_ptr<Mcts> mcts1;
    std::unique_ptr<Mcts> mcts2;

};

