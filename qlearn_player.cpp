#include "qlearn_player.h"

Pos QLearnPlayer::requestNextMove(std::optional<Pos> lastOpponentMovePosition) {
    Game simulationGame = game;

    startTimer();
    while (checkWithinTimeLimit()) {
        Game previousGameState = simulationGame;
        Pos move = chooseAction(game);
        if (!simulationGame.tryPlace(move, chess))
            throw std::runtime_error("Cannot place chess!");

        double reward = -1.0;
        auto winner = simulationGame.checkWin(move);
        if (winner.has_value()) {
            if (winner.value() == chess)
                reward = 100.0;
            else
                reward = -100.0;
        }

        learn(previousGameState.getBoard(), move, simulationGame.getBoard(), reward);
        if (winner.has_value() || simulationGame.checkDraw()) {
            simulationGame = game;
        }
    }

    auto boardMap = stateActionRewardMap[game.getBoard()];
    return std::max_element(std::begin(boardMap), std::end(boardMap), [] (auto &p1, auto &p2) -> bool {
        return p1.second < p2.second;
    })->first;
}

Pos QLearnPlayer::chooseAction(const Game &simulationGame) noexcept {
    auto availableMoves = simulationGame.getAvailableMoves();

    auto random = getRandomNumber<double>(0, 1);
    if (random < randomExploreFactor) {
        return *getRandomElement(availableMoves);
    } else {
        const Board &board = simulationGame.getBoard();

        std::unordered_set<Pos> maxRewardMoves;
        double maxReward = std::numeric_limits<double>::lowest();
        for (auto &move: availableMoves) {
            double reward = getRewardByStateAction(board, move);
            if (reward > maxReward) {
                maxRewardMoves.clear();
                maxRewardMoves.insert(move);
                maxReward = reward;
            } else if (reward == maxReward) {
                maxRewardMoves.insert(move);
            }
        }

        return *getRandomElement(maxRewardMoves);
    }
}

void QLearnPlayer::learn(const Board &oldBoard, const Pos &move, const Board &newBoard, double reward) noexcept {
    double maxReward = 0.0;
    auto boardMap = stateActionRewardMap.find(newBoard);
    if (boardMap != stateActionRewardMap.end()) {
        maxReward = std::max_element(std::begin(boardMap->second), std::end(boardMap->second),
                                     [] (auto &p1, auto &p2) -> bool {
            return p1.second < p2.second;
        })->second;
    }

    double oldReward = getRewardByStateAction(oldBoard, move);
    if (oldReward == 0.0) {
        setRewardByStateAction(oldBoard, move, reward);
    } else {
        setRewardByStateAction(oldBoard, move, oldReward + Alpha * (reward + Gamma * maxReward - oldReward));
    }
}