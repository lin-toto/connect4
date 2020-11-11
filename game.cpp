#include "game.h"

Game::Game(int X, int Y, int N) noexcept: board(X, Y), connectN(N) {}

const Board & Game::getBoard() const noexcept {
    return board;
}

bool Game::tryPlace(const Pos &pos, Chess chess) noexcept {
    try {
        Chess current = board.get(pos);
        if (current != Empty) return false;

        try {
            // If we are at the bottom of the chess board, there will be nothing underneath us,
            // and therefore the board.get method will throw exception, which we can safely
            // ignore here, as that actually indicates we can place.
            Chess down = board.get(pos + Pos{0, -1});
            if (down == Empty) return false;
        } catch (PosOutOfBoundsException &e) {}

        board.set(pos, chess);
    } catch (PosOutOfBoundsException &e) {
        return false;
    }

    return true;
}

std::optional<Chess> Game::checkWin(const Pos &lastPlacedPosition) const {
    if (auto winner = checkWinInternal(lastPlacedPosition, Pos{0, -1}, std::nullopt)) return winner;
    if (auto winner = checkWinInternal(lastPlacedPosition, Pos{1, 0}, Pos{-1, 0})) return winner;
    if (auto winner = checkWinInternal(lastPlacedPosition, Pos{1, -1}, Pos{-1, 1})) return winner;
    if (auto winner = checkWinInternal(lastPlacedPosition, Pos{1, 1}, Pos{-1, -1})) return winner;

    return std::nullopt;
}

std::optional<Chess> Game::checkWinInternal(
        const Pos &lastPlacedPosition, const Pos &direction1, const std::optional<Pos> &direction2) const {
    Chess checkType = board.get(lastPlacedPosition);
    if (checkType == Empty)
        throw std::runtime_error("Current position has no chess placed");

    int count = 1;
    Pos pos = lastPlacedPosition;
    while (count < connectN) {
        pos += direction1;
        try {
            Chess current = board.get(pos);
            if (current != checkType) break;
        } catch (PosOutOfBoundsException &e) {
            break;
        }
        count++;
    }

    if (direction2.has_value()) {
        pos = lastPlacedPosition;
        while (count < connectN) {
            pos += direction2.value();
            try {
                Chess current = board.get(pos);
                if (current != checkType) break;
            } catch (PosOutOfBoundsException &e) {
                break;
            }
            count++;
        }
    }

    return count == connectN ? std::optional<Chess>(checkType) : std::nullopt;
}