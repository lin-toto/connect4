#ifndef CONNECT4_PLAYER_H
#define CONNECT4_PLAYER_H

#include "game.h"

class BasePlayer {
public:
    explicit BasePlayer(const Game &currentGame) : game(currentGame) {};
    virtual Pos requestNextMove(Pos lastOpponentMovePosition) = 0;

protected:
    const Game &game;
};

#endif //CONNECT4_PLAYER_H
