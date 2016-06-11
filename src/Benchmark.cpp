#include <iostream>
#include <array>
#include <thread>
#include <array>
#include <cstdio>
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

            uint8_t winPercent = (100. * scores[1] / times);

            printf("1x%d vs 1x%d: %d%% %d\n",
                t * maxIterations2,
                maxIterations2,
                winPercent,
                scores[1]);

            if (t == 1) {
                t *= 2;
                continue;
            }

            std::array<uint16_t, 3> scores2 = testScores(t, maxIterations2);

            winPercent = (100. * scores2[1] / times);

            printf("%dx%d vs 1x%d: %d%% %d\n",
                t * maxIterations2,
                maxIterations2,
                maxIterations2,
                winPercent,
                scores2[1]);

            t *= 2;
        }        
    }

private:

    std::array<uint16_t, 3> testScores(uint8_t threads1, uint32_t maxIterations1) {
        std::array<uint16_t, 3> scores { 0, 0 , 0 };
        uint8_t threads2 = 1;
        for (uint16_t i=0; i < times; i++) {
//std::cout << "starting game\n";
            State* startState = new TicTacToe(dim, needed);
            SelfPlay play(threads1,
                threads2,
                timerPerActionSec,
                timerPerActionSec,
                maxIterations1,
                maxIterations2);

            uint8_t winner = play.play(startState);
//printf("winner %d\n", winner);
            scores[winner]++;

        }
        return scores;
    }

    uint8_t numCpu;

    uint16_t times{100};
    uint32_t maxIterations2{100};
    uint32_t timerPerActionSec{999};
    uint8_t dim{3};
    uint8_t needed{3};

};

int main() {
    Benchmark().run();
}


