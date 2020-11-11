#ifndef CONNECT4_PLAYER_H
#define CONNECT4_PLAYER_H

#include "game.h"

class BasePlayer {
public:
    explicit BasePlayer(const Game &currentGame, Chess myChess) : game(currentGame), chess(myChess) {};
    virtual Pos requestNextMove(std::optional<Pos> lastOpponentMovePosition) = 0;

protected:
    const Game &game;
    Chess chess;
};

#endif //CONNECT4_PLAYER_H
