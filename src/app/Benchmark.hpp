#pragma once

#include <iostream>
#include <array>
#include <thread>
#include <array>
#include <cstdio>
#include <string>

class Benchmark {
public:

    Benchmark(int dim, int needed, int times, int maxIterations2);

    void run();

private:

    std::array<uint16_t, 3> testScores(uint8_t threads1, uint32_t maxIterations1);

    const uint8_t numCpu;
    const uint16_t times;
    const uint32_t maxIterations2;
    const uint32_t timerPerActionSec{999000};
    const uint8_t dim;
    const uint8_t needed;

};

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cout << "specify: dim needed times maxIterations\n";
        exit(1);
    }
    int dim = std::stoi(argv[1]);
    int needed = std::stoi(argv[2]);
    int times = std::stoi(argv[3]);
    int maxIterations = std::stoi(argv[4]);
    Benchmark(dim, needed, times, maxIterations).run();
}


