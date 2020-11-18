#include "base_player.h"

ComputerPlayer::ComputerPlayer(const Game &currentGame, Chess myChess, int timeBudget): // NOLINT
    BasePlayer(currentGame, myChess), timeBudgetPerStep(timeBudget) {
    std::random_device rd;
    randomEngine = std::default_random_engine{rd()};
}

void ComputerPlayer::startTimer() noexcept {
    startTime = std::chrono::system_clock::now();
}

bool ComputerPlayer::checkWithinTimeLimit() const noexcept {
    auto currentTime = std::chrono::system_clock::now();
    return currentTime - startTime < timeBudgetPerStep;
}
