#ifndef CONNECT4_GAME_H
#define CONNECT4_GAME_H

#include "board.h"
#include <optional>
#include <stdexcept>
#include <vector>
#include <unordered_set>

class Game {
public:
    Game(int X, int Y, int N) noexcept;

    [[nodiscard]] const Board & getBoard() const noexcept;
    bool tryPlace(const Pos &pos, Chess chess) noexcept;
    void revertMove(const Pos &pos);
    [[nodiscard]] std::unordered_set<Pos> getAvailableMoves() const noexcept;

    // throws when lastPlacedPosition is out of bounds or has no chess at all.
    [[nodiscard]] std::optional<Chess> checkWin(const Pos &lastPlacedPosition) const;

private:
    Board board;
    std::vector<int> columnMaxY;
    int connectN;

    [[nodiscard]] std::optional<Chess> checkWinInternal(
            const Pos &lastPlacedPosition, const Pos &direction1, const std::optional<Pos> &direction2) const;
};

#endif //CONNECT4_GAME_H
