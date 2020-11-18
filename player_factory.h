#ifndef CONNECT4_PLAYER_FACTORY_H
#define CONNECT4_PLAYER_FACTORY_H

#include "players/base_player.h"
#include "players/human_player.h"
#include "players/mcts_player.h"
#include "players/qlearn_player.h"
#include <memory>

class PlayerFactory {
public:
    static std::unique_ptr<BasePlayer> create(PlayerType type, const Game &currentGame, Chess myChess, int timeBudget);
};

#endif
