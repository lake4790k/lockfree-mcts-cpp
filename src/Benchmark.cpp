#include <iostream>
#include <array>
#include <thread>
#include <array>
#include "SelfPlay.cpp"
#include "TicTacToe.cpp"

class Benchmark {
public:

    Benchmark():
        numCpu(std::thread::hardware_concurrency()) {}


    void run() {
        uint8_t t = 1;
        while (t <= 1) {
            std::array<uint16_t, 3> scores = testScores(1, t * maxIterations2);

            t *= 2;
        }        
    }

private:

    std::array<uint16_t, 3> testScores(uint8_t threads1, uint32_t maxIterations1) {
        std::array<uint16_t, 3> scores;
        uint8_t threads2 = 1;
        for (uint16_t i=0; i < times; i++) {
            State::Ptr startState = std::make_shared<TicTacToe>(dim, needed);
            SelfPlay play(startState,
                threads1,
                threads2,
                timerPerActionSec,
                timerPerActionSec,
                maxIterations1,
                maxIterations2);

            uint8_t winner = play.play();

            scores[winner]++;
        }
        return scores;
    }

    uint8_t numCpu;

    uint16_t times{1};
    uint32_t maxIterations2{100};
    uint32_t timerPerActionSec{999};
    uint8_t dim{5};
    uint8_t needed{4};

};

int main() {
    Benchmark().run();
}
