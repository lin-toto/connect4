#include "board.h"

Pos Pos::operator+(const Pos &other) const {
    return Pos{X + other.X, Y + other.Y};
}

Pos & Pos::operator+=(const Pos &other) {
    X += other.X;
    Y += other.Y;
    return *this;
}

PosOutOfBoundsException::PosOutOfBoundsException(Pos pos) noexcept: position(pos) {}
constexpr Pos PosOutOfBoundsException::getPos() const noexcept {
    return position;
}

Board::Board(int X, int Y) noexcept: sizeX(X), sizeY(Y)  {
    board = new Chess[sizeX * sizeY];
    memset(board, 0, sizeof(int) * sizeX * sizeY);
}

Board::Board(const Board &other) noexcept {
    sizeX = other.sizeX;
    sizeY = other.sizeY;
    board = new Chess[sizeX * sizeY];
    memcpy(board, other.board, sizeof(int) * sizeX * sizeY);
}

Board::~Board() noexcept {
    delete[] board;
}

constexpr int Board::getOffset(const Pos &pos) const {
    if (pos.Y < 0 || pos.Y >= sizeY || pos.X < 0 || pos.X >= sizeX)
        throw PosOutOfBoundsException(pos);

    return pos.Y * sizeX + pos.X;
}

Chess Board::get(const Pos &pos) const {
    return board[getOffset(pos)];
}

void Board::set(const Pos &pos, Chess chess) {
    board[getOffset(pos)] = chess;
}
