#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <cstdio>
#include "State.hpp"

class TicTacToe : public State {
public:
    static constexpr uint8_t NOT_OVER_YET = 99;
    static constexpr uint8_t DRAW = 0;

    TicTacToe(uint8_t dims, uint8_t needed);

    TicTacToe(TicTacToe* o, uint16_t action);

    TicTacToe(TicTacToe* o);

    std::vector<uint16_t>* getAvailableActions();

    bool isTerminal();

    uint8_t getPreviousAgent();

    uint8_t getRewardFor(uint8_t agent);

    State* takeAction(uint16_t action);

    void applyAction(uint16_t action);

    uint8_t getWinner();

    State* copy();

    void print();

private:
    TicTacToe( const TicTacToe& other ) = delete;
    TicTacToe& operator=( const TicTacToe& ) = delete; // non copyable

    uint8_t at(uint8_t r, uint8_t c);

    uint16_t updateWith(uint16_t action);

    const uint8_t dim;
    const uint8_t needed;

    std::vector<uint8_t> board;
    uint8_t agent{1};
    uint8_t winner{NOT_OVER_YET};
    uint32_t round{0};
};