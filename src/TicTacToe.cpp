#include <memory>
#include <cassert>
#include <vector>

#include "State.hpp"

class TicTacToe : public State {
public:
    const uint8_t NOT_OVER_YET = 99;
    const uint8_t DRAW = 0;

    TicTacToe(uint8_t dims, uint8_t needed):
        dim(dims),
        needed(needed),
        board(dims*dims, 0) {

        assert(dims >= needed);
    }

    TicTacToe(TicTacToe& o, uint16_t action):
        dim(o.dim),
        needed(o.needed),
        round(o.round),
        agent(3 - o.agent),
        board(o.board) {

        winner = updateWith(action);
    }

    std::shared_ptr<std::vector<uint16_t>> getAvailableActions() {
        uint16_t remaining = dim*dim - round;
        std::shared_ptr<std::vector<uint16_t>> actions = std::make_shared<std::vector<uint16_t>>(remaining);
        uint16_t idx = 0;
        for (uint8_t i=0; i < board.size(); i++) {
            if (board[i] == 0) {
                actions->at(idx++) = i;
            }
        }
        assert(idx==remaining);
        return actions;
    }

    bool isTerminal() {
        return winner < NOT_OVER_YET;
    }

    uint8_t getPreviousAgent() {
        return 3 - agent;
    }

    double getRewardFor(uint8_t agent) {
        assert(winner < NOT_OVER_YET);
        if (winner == DRAW) return 0.5;

        return winner == agent
            ? 1.
            : 0.;
    };

    State::Ptr takeAction(uint16_t action) {
        return std::make_shared<TicTacToe>(*this, action);
    }

    void applyAction(uint16_t action) {
        agent = 3 - agent;
        winner = updateWith(action);
    };

    uint8_t getWinner() {
        return winner;
    };

    State* copy() {
        return new TicTacToe(*this);
    };


private:

    uint8_t at(uint8_t r, uint8_t c) {
        return board[dim*r + c];
    }

    uint16_t updateWith(uint16_t action) {
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


    uint8_t dim;
    uint8_t needed;

    std::vector<uint8_t> board;
    uint8_t agent{1};
    uint8_t winner{NOT_OVER_YET};
    uint32_t round{0};
};