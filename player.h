#ifndef CONNECT4_PLAYER_H
#define CONNECT4_PLAYER_H

#include "game.h"
#include <random>
#include <chrono>

class BasePlayer {
public:
    explicit BasePlayer(const Game &currentGame, Chess myChess) : game(currentGame), chess(myChess) {};
    virtual Pos requestNextMove(std::optional<Pos> lastOpponentMovePosition) = 0;
    virtual bool isInteractive() const noexcept = 0;
protected:
    const Game &game;
    Chess chess;
};

class ComputerPlayer : public BasePlayer {
public:
    explicit ComputerPlayer(const Game &currentGame, Chess myChess, int timeBudget);
    [[nodiscard]] bool isInteractive() const noexcept override { return false; }
protected:
    std::chrono::milliseconds timeBudgetPerStep;
    std::chrono::time_point<std::chrono::system_clock> startTime;

    std::default_random_engine randomEngine;

    [[nodiscard]] constexpr Chess getMoveChessType(bool isCurrentPlayer) const noexcept {
        if (chess == Player1) {
            return isCurrentPlayer ? Player1 : Player2;
        } else {
            return isCurrentPlayer ? Player2 : Player1;
        }
    }

    void startTimer() noexcept;
    [[nodiscard]] bool checkWithinTimeLimit() const noexcept;

    template <class T>
    [[nodiscard]] T getRandomNumber(T lower, T upper) noexcept {
        std::uniform_int_distribution<T> uniformDist(lower, upper);
        return uniformDist(randomEngine);
    }

    template <class T>
    [[nodiscard]] auto getRandomElement(T &set) noexcept {
        return std::next(std::begin(set), getRandomNumber<int>(0, static_cast<int>(set.size()) - 1));
    }
};

#endif //CONNECT4_PLAYER_H
