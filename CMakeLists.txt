cmake_minimum_required (VERSION 2.6)
project (Mcts)

set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++")

file(GLOB SOURCES "src/*.cpp")
 
add_executable(mcts ${SOURCES})