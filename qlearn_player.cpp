#include "qlearn_player.h"

Pos QLearnPlayer::requestNextMove(std::optional<Pos> lastOpponentMovePosition) {
    Game simulationGame = game;

    startTimer();
    int stepCount = 0;
    while (checkWithinTimeLimit()) {
        stepCount++;
        Board previousBoard = simulationGame.getBoard();
        Pos myMove = chooseAction(simulationGame, true);
        if (!simulationGame.tryPlace(myMove, getMoveChessType(true)))
            throw std::runtime_error("Cannot place chess!");

        Pos opponentMove = chooseAction(simulationGame, false);
        if (!simulationGame.tryPlace(opponentMove, getMoveChessType(false)))
            throw std::runtime_error("Cannot place chess!");

        double reward = 0;
        auto winner = simulationGame.checkWin(myMove);
        if (winner.has_value()) {
            if (winner.value() == chess)
                reward = 1.0;
            else {
                if (stepCount == 1) {
                    reward = -100000.0;
                } else {
                    reward = -2.0;
                }
            }
        }

        learn(previousBoard, myMove, simulationGame.getBoard(), reward);
        if (winner.has_value() || simulationGame.checkDraw() || stepCount >= 4) {
            simulationGame = game;
            stepCount = 0;
        }
    }

    auto boardMap = stateActionRewardMap[game.getBoard()];
    return std::max_element(std::begin(boardMap), std::end(boardMap), [] (auto &p1, auto &p2) -> bool {
        return p1.second < p2.second;
    })->first;
}

Pos QLearnPlayer::chooseAction(const Game &simulationGame, bool isCurrentPlayer) noexcept {
    auto availableMoves = simulationGame.getAvailableMoves();

    auto random = getRandomNumber<double>(0, 1);
    if (random < randomExploreFactor || !isCurrentPlayer) {
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