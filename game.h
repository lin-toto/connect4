#ifndef CONNECT4_GAME_H
#define CONNECT4_GAME_H

#include "board.h"
#include <optional>
#include <stdexcept>

class Game {
public:
    Game(int X, int Y, int N) noexcept;

    [[nodiscard]] const Board & getBoard() const noexcept;
    bool tryPlace(const Pos &pos, Chess chess) noexcept;

    // throws when lastPlacedPosition is out of bounds or has no chess at all.
    [[nodiscard]] std::optional<Chess> checkWin(const Pos &lastPlacedPosition) const;

private:
    Board board;
    int connectN;

    [[nodiscard]] std::optional<Chess> checkWinInternal(
            const Pos &lastPlacedPosition, const Pos &direction1, const std::optional<Pos> &direction2) const;
};

#endif //CONNECT4_GAME_H
