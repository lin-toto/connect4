#ifndef CONNECT4_HUMAN_PLAYER_H
#define CONNECT4_HUMAN_PLAYER_H

#include "player.h"

class HumanPlayer : public BasePlayer {
    [[nodiscard]] bool isInteractive() const noexcept override { return true; }
    Pos requestNextMove(std::optional<Pos> lastOpponentMovePosition) override {
        throw std::runtime_error("HumanPlayer is interactive");
    }
};

#endif //CONNECT4_HUMAN_PLAYER_H
