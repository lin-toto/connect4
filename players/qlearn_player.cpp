#include "qlearn_player.h"

#include <iostream>
Pos QLearnPlayer::requestNextMove([[maybe_unused]] std::optional<Pos> lastOpponentMovePosition) {
    Game simulationGame = game;

    startTimer();
    int stepCount = 0;

    std::optional<Chess> winner;
    while (checkWithinTimeLimit()) {
        winner = std::nullopt;
        stepCount++;
        Board previousBoard = simulationGame.getBoard();

        auto myMove = chooseAction(simulationGame, true);
        if (myMove.has_value()) {
            if (!simulationGame.tryPlace(myMove.value(), getMoveChessType(true)))
                throw std::runtime_error("Cannot place chess!");

            winner = simulationGame.checkWin(myMove.value());

            if (!winner.has_value()) {
                auto opponentMove = chooseAction(simulationGame, false);
                if (opponentMove.has_value()) {
                    if (!simulationGame.tryPlace(opponentMove.value(), getMoveChessType(false)))
                        throw std::runtime_error("Cannot place chess!");

                    winner = simulationGame.checkWin(opponentMove.value());
                }
            }
        }

        double reward = 0;
        if (winner.has_value()) {
            if (winner.value() == chess)
                reward = 1.0;
            else {
                if (stepCount == 1) {
                    reward = -10000000.0;
                } else if (stepCount == 2) {
                    reward = -1000.0;
                } else {
                    reward = -1.0;
                }
            }
        }

        if (myMove.has_value())
            learn(previousBoard, myMove.value(), simulationGame.getBoard(), reward);

        if (winner.has_value() || !myMove.has_value()) {
            simulationGame = game;
            stepCount = 0;
        }
    }

    auto boardMap = stateActionRewardMap[game.getBoard()];
    return std::max_element(std::begin(boardMap), std::end(boardMap), [] (auto &p1, auto &p2) -> bool {
        return p1.second < p2.second;
    })->first;
}

std::optional<Pos> QLearnPlayer::chooseAction(const Game &simulationGame, bool isCurrentPlayer) noexcept {
    auto availableMoves = simulationGame.getAvailableMoves();
    if (availableMoves.empty())
        return std::nullopt;

    auto random = getRandomNumber<double>(0, 1);
    if (random < randomExploreFactor) {
        return *getRandomElement(availableMoves);
    } else {
        const Board &board = simulationGame.getBoard();

        std::unordered_set<Pos> maxRewardMoves;
        double maxReward = std::numeric_limits<double>::lowest();
        for (auto &move: availableMoves) {
            double reward = getRewardByStateAction(board, move) * (isCurrentPlayer ? 1 : -1);
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