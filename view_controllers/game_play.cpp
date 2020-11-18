#include "game_play.h"

GamePlayViewController::GamePlayViewController(PlayerType player1Type, PlayerType player2Type, int X, int Y, int N):
        sizeX(X), sizeY(Y), game(X, Y, N) {
    // Initialize two colors for board borders, and highlighted chess.
    init_pair(0xC0, COLOR_CYAN, COLOR_BLACK);
    init_pair(0xC1, COLOR_GREEN, COLOR_BLACK);
    init_pair(0xD0, COLOR_WHITE, COLOR_BLUE);

    player1 = PlayerFactory::create(player1Type, game, Player1, aiTimeBudget);
    player2 = PlayerFactory::create(player2Type, game, Player2, aiTimeBudget);
}

void GamePlayViewController::render() {
    release();

    int x, y;
    std::tie(x, y) = UI::getWindowSize();

    if (x < sizeX * 4 + 1 + boardPaddingX || y < sizeY * 2 + 1 + boardPaddingY) {
        mvprintw(1, 2, "Window too small");
        mvprintw(2, 2, "Please enlarge terminal");
        return;
    }

    boardWindow = newwin(sizeY * 2 + 1, sizeX * 4 + 1, boardPaddingY / 2, boardPaddingX / 2);
    keypad(boardWindow, true);
    drawBoard(boardWindow, game.getBoard(), cursorPosition);
    wrefresh(boardWindow);

    mvprintw(y - 1, 0, "Press Arrow Keys to navigate the board, <ENTER> to place, <F1> to exit");
}

void GamePlayViewController::drawBoard(WINDOW *window, const Board &board, Pos pos) noexcept {
    for (int i = 0; i <= sizeX; i++) {
        for (int j = 0; j <= sizeY; j++) {
            // Draw horizontal border, use blue color on top/bottom.
            wattron(window, COLOR_PAIR((j == 0 || j == sizeY) ? 0xC0 : 0xC1));
            mvwaddstr(window, j * 2, i * 4 + 1, "---");
            wattroff(window, COLOR_PAIR(0xC0));
            wattroff(window, COLOR_PAIR(0xC1));

            // Draw vertical border, use blue color on left/right.
            wattron(window, COLOR_PAIR((i == 0 || i == sizeX) ? 0xC0 : 0xC1));
            mvwaddch(window, j * 2 + 1, i * 4, '|');
            wattroff(window, COLOR_PAIR(0xC0));
            wattroff(window, COLOR_PAIR(0xC1));

            // Draw border crossings, use blue color on edges.
            wattron(window, COLOR_PAIR((i == 0 || i == sizeX || j == 0 || j == sizeY) ? 0xC0 : 0xC1));
            mvwaddch(window, j * 2, i * 4, '+');
            wattroff(window, COLOR_PAIR(0xC0));
            wattroff(window, COLOR_PAIR(0xC1));

        }
    }

    for (int i = 0; i < sizeX; i++) {
        for (int j = 0; j < sizeY; j++) {
            Chess c = board.get(Pos{i, j});
            updateChess(window, Pos{i, j}, c, i == pos.X && j == pos.Y);
        }
    }
}

void GamePlayViewController::updateChess(WINDOW *window, Pos pos, Chess chess, bool highlight, bool refresh) noexcept {
    // Y coordinate is flipped here, because y starts at bottom in the chessboard.

    if (highlight) wattron(window, COLOR_PAIR(0xD0));
    mvwaddstr(window, (sizeY - pos.Y - 1) * 2 + 1, pos.X * 4 + 1, "   ");
    if (chess != Empty) mvwaddwstr(window, (sizeY - pos.Y - 1) * 2 + 1, pos.X * 4 + 2, getChessText(chess));
    wattroff(window, COLOR_PAIR(0xD0));

    if (refresh) wrefresh(window);
}

bool GamePlayViewController::tryMoveCursor(Pos newPosition) noexcept {
    if (newPosition.X >= 0 && newPosition.X < sizeX && newPosition.Y >= 0 && newPosition.Y < sizeY) {
        updateChess(boardWindow, cursorPosition, game.getBoard().get(cursorPosition), false);
        updateChess(boardWindow, newPosition, game.getBoard().get(newPosition), true);
        wrefresh(boardWindow);
        cursorPosition = newPosition;
        return true;
    }

    else return false;
}

void GamePlayViewController::checkEvent() {
    if (boardWindow != nullptr) {
        wtimeout(boardWindow, 0);
        int c = wgetch(boardWindow);
        if (c > 0) handleKeyboardEvent(c);

        if (currentPlacedPosition.load().has_value()) {
            Pos pos = currentPlacedPosition.load().value();
            if (!game.tryPlace(pos, currentPlayer)) {
                showDialog("You cannot place at this position!");
                currentPlacedPosition.store(std::nullopt);
            } else {
                tryMoveCursor(pos);
                currentPlayer = currentPlayer == Player1 ? Player2 : Player1;

                auto winner = game.checkWin(pos);
                if (winner.has_value()) {
                    std::ostringstream winnerText;
                    winnerText << "Winner is Player " << (winner.value() == Player1 ? 1 : 2) << "!";
                    showDialog(winnerText.str());
                    boardFinal = true;
                } else if (game.checkDraw()) {
                    showDialog("Draw round!");
                    boardFinal = true;
                }

                currentPlacedPosition.store(std::nullopt);

                if (!getCurrentPlayerObject()->isInteractive()) {
                    // is AI, launch new thread for calculation
                    std::async(std::launch::async, [this, pos]() { // NOLINT
                        BasePlayer *player = getCurrentPlayerObject();
                        currentPlacedPosition.store(player->requestNextMove(pos));
                    });
                }
            }
        }
    }
}


void GamePlayViewController::handleKeyboardEvent(int key) noexcept {
    if (boardWindow != nullptr) {
        switch (key) {
            default:
                break;
            case KEY_F1:
                UI::getInstance()->stateTransition(StateMainMenu);
                break;
            case 10: // Enter
                if (handleDialogEvent()) break;

                if (!boardFinal) currentPlacedPosition.store(cursorPosition);
                break;
            case KEY_UP:
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:
                Pos offset{};
                switch (key) {
                    case KEY_UP: offset = Pos{0, 1}; break;
                    case KEY_DOWN: offset = Pos{0, -1}; break;
                    case KEY_LEFT: offset = Pos{-1, 0}; break;
                    case KEY_RIGHT: offset = Pos{1, 0}; break;
                    default: break;
                }
                tryMoveCursor(cursorPosition + offset);
                break;
        }
    }
}

void GamePlayViewController::release() noexcept {
    handleDialogEvent(false); // Remove any dialogs

    if (boardWindow != nullptr) {
        delwin(boardWindow);
        boardWindow = nullptr;
    }
}