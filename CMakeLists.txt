cmake_minimum_required (VERSION 2.6)
project (Mcts)

set(CMAKE_CXX_FLAGS "-std=c++14 -pthread -O4 -g")

file(GLOB SOURCES src/*.cpp)
add_library(mcts ${SOURCES})

add_executable( benchmark src/app/Benchmark.cpp )
target_link_libraries( benchmark mcts)

add_executable( testThreads src/test/testThreads.cpp )
target_link_libraries( testThreads mcts)

add_executable( testObjectPool src/test/testObjectPool.cpp )
target_link_libraries( testObjectPool mcts)
