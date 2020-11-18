#include "player_factory.h"

std::unique_ptr<BasePlayer> PlayerFactory::create(PlayerType type, const Game &currentGame, Chess myChess, int timeBudget) {
    switch (type) {
        case Human:
            return std::make_unique<HumanPlayer>(currentGame, myChess);
        case MCTS:
            return std::make_unique<MCTSPlayer>(currentGame, myChess, timeBudget);
        case QLearn:
            return std::make_unique<QLearnPlayer>(currentGame, myChess, timeBudget);
    }
}