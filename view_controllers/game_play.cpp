#include "game_play.h"

GamePlayViewController::GamePlayViewController(PlayerType player1Type, PlayerType player2Type, int X, int Y, int N):
        sizeX(X), sizeY(Y), game(X, Y, N) {
    // Initialize two colors for board borders, and highlighted chess.
    init_pair(0xC0, COLOR_CYAN, COLOR_BLACK);
    init_pair(0xC1, COLOR_GREEN, COLOR_BLACK);
    init_pair(0xD0, COLOR_WHITE, COLOR_BLUE);

    player1 = PlayerFactory::create(player1Type, game, Player1, aiTimeBudget);
    player2 = PlayerFactory::create(player2Type, game, Player2, aiTimeBudget);

    requestNextMove(std::nullopt);
}

void GamePlayViewController::render() {
    release();

    int x, y;
    std::tie(x, y) = UI::getWindowSize();
    int boardWidth = sizeX * 4 + 1, boardHeight = sizeY * 2 + 1;
    bool renderSidebar = x > boardWidth + boardPaddingX + sidebarWidth;

    if (x < boardWidth + boardPaddingX || y < boardHeight + boardPaddingY) {
        mvprintw(1, 2, "Window too small");
        mvprintw(2, 2, "Please enlarge terminal");
        return;
    }

    int windowPosY = (y - boardPaddingY - boardHeight) / 2;
    int windowPosX = (x - boardPaddingX - boardWidth - (renderSidebar ? sidebarWidth : 0)) / 2;
    boardWindow = newwin(boardHeight, boardWidth, windowPosY, windowPosX);
    keypad(boardWindow, true);
    drawBoard(boardWindow, game.getBoard(), cursorPosition);
    wrefresh(boardWindow);

    // TODO: make this sidebar window
    if (renderSidebar) {
        // Draw border for sidebar.
        mvvline(0, x - sidebarWidth, ACS_VLINE, y);

        attron(COLOR_PAIR(0xC0));
        mvhline(y / 2 - 2, x - sidebarWidth + 1, '-', sidebarWidth - 1);
        mvhline(y / 2 + 2, x - sidebarWidth + 1, '-', sidebarWidth - 1);
        attroff(COLOR_PAIR(0xC0));

        sidebarWindow = newwin(3, sidebarWidth - 2, y / 2 - 1, x - sidebarWidth + 2);
    }
    updateInformation();

    mvprintw(y - 1, 0, "Press Arrow Keys to navigate the board, <ENTER> to place, <F1> to exit");
}

void GamePlayViewController::updateTimer() noexcept {
    if (sidebarWindow != nullptr) {
        std::ostringstream buf;
        buf.fill('0');
        buf << "Elapsed "
            << std::setw(2) << currentElapsedSeconds.count() / 60 << ":"
            << std::setw(2) << currentElapsedSeconds.count() % 60;
        mvwaddstr(sidebarWindow, 2, 0, buf.str().c_str());

        wrefresh(sidebarWindow);
    }
}

void GamePlayViewController::updateInformation() noexcept {
    if (sidebarWindow != nullptr) {
        wclear(sidebarWindow);

        if (winner.has_value()) {
            mvwaddwstr(sidebarWindow,0, 0, getPlayerDescription(winner.value()).c_str());
            mvwaddstr(sidebarWindow,2, 0, "Winner!");
        } else if (drawGame) {
            mvwaddstr(sidebarWindow,1, 0, "Draw round!");
        } else {
            mvwaddwstr(sidebarWindow, 0, 0, getPlayerDescription(currentPlayer).c_str());
            if (getCurrentPlayerObject()->isInteractive()) {
                mvwaddstr(sidebarWindow, 1, 0, "Your turn");
            } else {
                mvwaddstr(sidebarWindow,1, 0, "Thinking...");
            }
            updateTimer();
        }

        wrefresh(sidebarWindow);
    }
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

        if (!isBoardFinal()) {
            auto elapsed = getElapsedTime();
            if (elapsed != currentElapsedSeconds) {
                currentElapsedSeconds = elapsed;
                updateTimer();
            }
            
            playGame();
        }
    }
}

void GamePlayViewController::playGame() noexcept {
    if (currentPlacedPosition.load().has_value()) {
        Pos pos = currentPlacedPosition.load().value();
        if (!game.tryPlace(pos, currentPlayer)) {
            showDialog("You cannot place at this position!");
            currentPlacedPosition.store(std::nullopt);
        } else {
            tryMoveCursor(pos); // Function also updates chess display

            currentPlayer = currentPlayer == Player1 ? Player2 : Player1;
            currentPlacedPosition.store(std::nullopt);

            winner = game.checkWin(pos);
            if (winner.has_value()) {
                std::ostringstream winnerText;
                winnerText << "Winner is Player " << (winner.value() == Player1 ? 1 : 2) << "!";
                showDialog(winnerText.str());
            } else {
                drawGame = game.checkDraw();
                if (drawGame) {
                    showDialog("Draw round!");
                } else {
                    requestNextMove(pos);
                }
            }

            updateInformation();
        }
    }
}

void GamePlayViewController::requestNextMove(std::optional<Pos> lastPlacedPosition) noexcept {
    resetTimer();

    if (!getCurrentPlayerObject()->isInteractive()) {
        // is AI, launch new thread for calculation
        aiPlayerFuture = std::async(std::launch::async, [this, lastPlacedPosition]() { // NOLINT
            BasePlayer *player = getCurrentPlayerObject();
            currentPlacedPosition.store(player->requestNextMove(lastPlacedPosition));
        });
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

                if (!isBoardFinal())
                    currentPlacedPosition.store(cursorPosition);
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

    if (sidebarWindow != nullptr) {
        delwin(sidebarWindow);
        sidebarWindow = nullptr;
    }
}

GamePlayViewController::~GamePlayViewController() noexcept {
    free_pair(0xC0);
    free_pair(0xC1);
    free_pair(0xD0);

    release();
}