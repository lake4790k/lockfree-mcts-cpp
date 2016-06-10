#ifndef _STATE
#define _STATE

#include <vector>
#include <memory>

class State {
public:
    typedef std::shared_ptr<State> Ptr;

    virtual ~State();

    virtual std::shared_ptr<std::vector<uint16_t>> getAvailableActions() = 0;

    virtual bool isTerminal() = 0;

    virtual uint8_t getPreviousAgent() = 0;

    virtual double getRewardFor(uint8_t agent) = 0;

    virtual State::Ptr takeAction(uint16_t action) = 0;

    virtual void applyAction(uint16_t action) = 0;

    virtual uint8_t getWinner() = 0;

    virtual State::Ptr copy() = 0;
};

#endif