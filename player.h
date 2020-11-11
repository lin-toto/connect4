#ifndef CONNECT4_PLAYER_H
#define CONNECT4_PLAYER_H

#include "board.h"

class BasePlayer {
public:
    BasePlayer(const Board &currentBoard) : board(currentBoard) {};
    virtual Chess requestNextMove(Pos lastOpponentMovePosition) = 0;

protected:
    const Board &board;
};

#endif //CONNECT4_PLAYER_H
