#include <iostream>
#include <array>
#include <thread>
#include <array>
#include <cstdio>
#include <string>
#include "SelfPlay.hpp"
#include "TicTacToe.hpp"
#include "Benchmark.hpp"


Benchmark::Benchmark(int dim, int needed, int times, int maxIterations2):
    dim(dim),
    needed(needed),
    times(times),
    maxIterations2(maxIterations2),
    numCpu(std::thread::hardware_concurrency()) {}


void Benchmark::run() {
    uint8_t t = 1;
    while (t <= numCpu) {
        std::array<uint16_t, 3> scores = testScores(1, t * maxIterations2);

        uint8_t winPercent = (100. * scores[1] / times);

        printf("1x%d vs 1x%d: %d%% [%d %d %d]\n",
            t * maxIterations2,
            maxIterations2,
            winPercent,
            scores[0], scores[1], scores[2]);

        if (t == 1) {
            t *= 2;
            continue;
        }

        std::array<uint16_t, 3> scores2 = testScores(t, maxIterations2);

        winPercent = (100. * scores2[1] / times);

        printf("%dx%d vs 1x%d: %d%% [%d %d %d]\n",
            t,
            maxIterations2,
            maxIterations2,
            winPercent,
            scores2[0], scores2[1], scores2[2]);

        t *= 2;
    }        
}


std::array<uint16_t, 3> Benchmark::testScores(uint8_t threads1, uint32_t maxIterations1) {
    std::array<uint16_t, 3> scores { 0, 0 , 0 };
    uint8_t threads2 = 1;
    for (uint16_t i=0; i < times; i++) {
        State* startState = new TicTacToe(dim, needed);
        SelfPlay play(threads1,
            threads2,
            timerPerActionSec,
            timerPerActionSec,
            maxIterations1,
            maxIterations2);

        uint8_t winner = play.play(startState);
        scores[winner]++;

    }
    return scores;
}


