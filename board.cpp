#include "board.h"

Pos Pos::operator+(const Pos &other) const {
    return Pos{X + other.X, Y + other.Y};
}

Pos & Pos::operator+=(const Pos &other) {
    X += other.X;
    Y += other.Y;
    return *this;
}

bool Pos::operator==(const Pos &other) const {
    return X == other.X && Y == other.Y;
}

std::size_t std::hash<Pos>::operator()(const Pos &pos) const {
    return std::hash<int>()(pos.X) ^ (std::hash<int>()(pos.Y) << 1); // NOLINT
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

std::pair<int, int> Board::getSize() const {
    return std::make_pair(sizeX, sizeY);
}

void Board::set(const Pos &pos, Chess chess) {
    board[getOffset(pos)] = chess;
}
