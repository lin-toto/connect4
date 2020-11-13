#ifndef CONNECT4_BOARD_H
#define CONNECT4_BOARD_H

#include <cstring>
#include <exception>
#include <functional>
#include <utility>

enum Chess: unsigned char {
    Empty = 0,
    Player1,
    Player2
};

struct Pos {
    int X, Y;
    Pos operator+(const Pos &other) const;
    Pos & operator+=(const Pos &other);
    bool operator==(const Pos &other) const;
};

template <>
struct std::hash<Pos> {
    std::size_t operator()(const Pos &pos) const;
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
    bool operator==(const Board &other) const;
    [[nodiscard]] Chess get(const Pos &pos) const;
    [[nodiscard]] constexpr std::pair<int, int> getSize() const {
        return std::make_pair(sizeX, sizeY);
    }
    friend class Game;
    friend class std::hash<Board>;
private:
    int sizeX, sizeY;
    Chess *board;
    void set(const Pos &pos, Chess chess);
    [[nodiscard]] constexpr int getOffset(const Pos &pos) const;
};

template <>
struct std::hash<Board> {
    std::size_t operator()(const Board &board) const;
};

#endif //CONNECT4_BOARD_H
