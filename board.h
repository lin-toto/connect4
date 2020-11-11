#ifndef CONNECT4_BOARD_H
#define CONNECT4_BOARD_H

#include <cstring>
#include <exception>

enum Chess {
    Empty = 0,
    Player1,
    Player2
};

struct Pos {
    int X, Y;
    Pos operator+(const Pos &other) const;
    Pos & operator+=(const Pos &other);
};

class PosOutOfBoundsException : public std::exception {
public:
    explicit PosOutOfBoundsException(Pos pos) noexcept;
    [[nodiscard]] constexpr Pos getPos() const noexcept;
private:
    Pos position;
};

class Game;
class Board {
public:
    Board(int X, int Y) noexcept;
    Board(const Board &other) noexcept;
    ~Board() noexcept;
    [[nodiscard]] Chess get(const Pos &pos) const;
    friend class Game;
private:
    int sizeX, sizeY;
    Chess *board;
    void set(const Pos &pos, Chess chess);
    [[nodiscard]] constexpr int getOffset(const Pos &pos) const;
};

#endif //CONNECT4_BOARD_H
