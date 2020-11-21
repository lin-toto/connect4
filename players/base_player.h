#ifndef CONNECT4_BASE_PLAYER_H
#define CONNECT4_BASE_PLAYER_H

#include "game.h"
#include <random>
#include <chrono>
#include <atomic>

enum PlayerType: char {
    Human = 0,
    MCTS = 1,
    QLearn = 2
};

class BasePlayer {
public:
    explicit BasePlayer(const Game &currentGame, Chess myChess) : game(currentGame), chess(myChess) {};
    virtual Pos requestNextMove(std::optional<Pos> lastOpponentMovePosition) = 0;
    virtual void requestEarlyTermination() noexcept {}
    [[nodiscard]] virtual bool isInteractive() const noexcept = 0;
    virtual ~BasePlayer() = default;
protected:
    const Game &game;
    Chess chess;
};

class ComputerPlayer : public BasePlayer {
public:
    explicit ComputerPlayer(const Game &currentGame, Chess myChess, int timeBudget): // NOLINT
            BasePlayer(currentGame, myChess), timeBudgetPerStep(timeBudget) {
        std::random_device rd;
        randomEngine = std::default_random_engine{rd()};
    }
    void requestEarlyTermination() noexcept override { terminateFlag.store(true); }
    [[nodiscard]] bool isInteractive() const noexcept override { return false; }
protected:
    std::chrono::milliseconds timeBudgetPerStep;
    std::chrono::time_point<std::chrono::system_clock> startTime;

    std::default_random_engine randomEngine;

    std::atomic_bool terminateFlag = false;

    [[nodiscard]] constexpr Chess getMoveChessType(bool isCurrentPlayer) const noexcept {
        if (chess == Player1) {
            return isCurrentPlayer ? Player1 : Player2;
        } else {
            return isCurrentPlayer ? Player2 : Player1;
        }
    }

    constexpr void startTimer() noexcept { startTime = std::chrono::system_clock::now(); }
    [[nodiscard]] bool checkWithinTimeLimit() const noexcept {
        auto currentTime = std::chrono::system_clock::now();
        return !terminateFlag.load() && currentTime - startTime < timeBudgetPerStep;
    }

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

#endif //CONNECT4_BASE_PLAYER_H
