cmake_minimum_required (VERSION 2.6)
project (Mcts)

set(CMAKE_CXX_FLAGS "-std=c++14 -pthread -O4")

file(GLOB SOURCES "src/*.cpp")
 
add_executable(mcts ${SOURCES})