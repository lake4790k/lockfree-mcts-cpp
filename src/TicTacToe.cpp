#include <iostream>
#include <cassert>
#include <vector>
#include <cstdio>
#include "State.hpp"
#include "TicTacToe.hpp"

TicTacToe::TicTacToe(uint8_t dims, uint8_t needed):
    dim(dims),
    needed(needed),
    board(dims*dims, 0),
    winner(NOT_OVER_YET) {

    assert(dims >= needed);
}

TicTacToe::TicTacToe(TicTacToe* o, uint16_t action):
    dim(o->dim),
    needed(o->needed),
    round(o->round),
    agent(3 - o->agent),
    board(o->board) {

    winner = updateWith(action);
}

TicTacToe::TicTacToe(TicTacToe* o):
    dim(o->dim),
    needed(o->needed),
    round(o->round),
    agent(o->agent),
    board(o->board),
    winner(o->winner) { }

std::vector<uint16_t>* TicTacToe::getAvailableActions() {
    uint16_t remaining = dim*dim - round;
    std::vector<uint16_t>* actions = new std::vector<uint16_t>(remaining);
    uint16_t idx = 0;
    for (uint8_t i=0; i < board.size(); i++) {
        if (board[i] == 0) {
            actions->at(idx++) = i;
        }
    }
    if (idx!=remaining) throw std::logic_error("idx!=remaining");

    return actions;
}

bool TicTacToe::isTerminal() {
    return winner < NOT_OVER_YET;
}

uint8_t TicTacToe::getPreviousAgent() {
    return 3 - agent;
}

uint8_t TicTacToe::getRewardFor(uint8_t agent) {
    assert(winner < NOT_OVER_YET);
    if (winner == DRAW) return 1;

    return winner == agent
        ? 2
        : 0;
};

State* TicTacToe::takeAction(uint16_t action) {
    return new TicTacToe(this, action);
}

void TicTacToe::applyAction(uint16_t action) {
    agent = 3 - agent;
    winner = updateWith(action);
};

uint8_t TicTacToe::getWinner() {
    return winner;
};

State* TicTacToe::copy() {
    return new TicTacToe(this);
};

void TicTacToe::print() {
    printf("player=%d\n", agent);
    for (int r = 0; r < dim; r++) {
        for (int c = 0; c < dim; c++) {
            switch (at(r, c)) {
                case 0:
                    std::cout << ".";
                    break;
                case 1:
                    std::cout << "X";
                    break;
                case 2:
                    std::cout << "O";
                    break;
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}


uint8_t TicTacToe::at(uint8_t r, uint8_t c) {
    return board[dim*r + c];
}

uint16_t TicTacToe::updateWith(uint16_t action) {
    uint8_t prevAgent = 3 - agent;
    uint8_t row = action / dim;
    uint8_t col = action % dim;
    round++;
    board[action] = prevAgent;

    uint16_t contiguous = 0;
    for (uint8_t r=0; r < dim; r++) {
        if (at(r,col) != prevAgent) {
            contiguous = 0;
        } else {
            if (++contiguous == needed) 
                return prevAgent;
        }
    }

    contiguous = 0;
    for (uint8_t c=0; c < dim; c++) {
        if (at(row,c) != prevAgent) {
            contiguous = 0;
        } else {
            if (++contiguous == needed) 
                return prevAgent;
        }
    }

    if (row == col) {
        contiguous = 0;
        for (uint8_t x = 0; x < dim; x++) {
            if (at(x, x) != prevAgent) {
                contiguous = 0;
            } else {
                if (++contiguous == needed)
                    return prevAgent;
            }
        }
    }

    if (row == dim - 1 - col) {
        contiguous = 0;
        for (uint8_t x = 0; x < dim; x++) {
            if (at(x, dim - 1 - x) != prevAgent) {
                contiguous = 0;
            } else {
                if (++contiguous == needed)
                    return prevAgent;
            }
        }
    }

    return round == dim * dim
        ? DRAW
        : NOT_OVER_YET;
}


