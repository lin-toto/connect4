#include <iostream>

#include "game.h"
#include "board.h"
#include <optional>

void printBoard(Game &game, int X, int Y) {
    Board &board = game.getBoard();
    for (int i = Y - 1; i >= 0; i--) {
        for (int j = 0; j < X; j++) {
            Chess current = board.get(Pos{j, i});
            switch (current) {
                case Empty:
                    std::cout << ".";
                    break;
                case Player1:
                    std::cout << "X";
                    break;
                case Player2:
                    std::cout << "O";
                    break;
            }
        }
        std::cout << std::endl;
    }
}

int main() {
    int X, Y, N;
    std::cin >> X >> Y >> N;

    Game game(X, Y, N);
    int x, y;
    int currentPlayer = 1;
    std::optional<Chess> winner;
    do {
        printBoard(game, X, Y);
        std::cin >> x >> y;

        if (!game.tryPlace(Pos{x, y}, currentPlayer == 1 ? Player1 : Player2)) {
            std::cout << "Cannot place!" << std::endl;
        } else {
            std::cout << "Player " << currentPlayer << " placed!" << std::endl;
            currentPlayer = currentPlayer == 1 ? 2 : 1;
            winner = game.checkWin(Pos{x, y});
        }
    } while (!winner.has_value());

    std::cout << "Winner is " << winner.value() << std::endl;
    return 0;
}