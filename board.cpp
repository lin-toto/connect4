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

bool Board::operator==(const Board &other) const {
    if (sizeX != other.sizeX || sizeY != other.sizeY) return false;
    for (int i = 0; i < sizeX * sizeY; i++) {
        if (board[i] != other.board[i])
            return false;
    }

    return true;
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

std::size_t std::hash<Board>::operator()(const Board &board) const {
    std::size_t hash = 0;

    unsigned long long data = 0;
    for (int i = 0; i < board.sizeX * board.sizeY; i++) {
        data <<= 2; // NOLINT
        data |= board.board[i];
        if (i % 32 == 31) {
            hash <<= 1; // NOLINT
            hash ^= std::hash<unsigned long long>()(data);
            data = 0;
        }
    }

    hash ^= std::hash<unsigned long long>()(data);
    return hash;
}