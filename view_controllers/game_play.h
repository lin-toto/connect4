#ifndef CONNECT4_GAME_PLAY_H
#define CONNECT4_GAME_PLAY_H

#include "base_view_controller.h"
#include "players/base_player.h"
#include "player_factory.h"
#include "ui.h"
#include "game.h"
#include <memory>
#include <atomic>
#include <optional>
#include <string>
#include <sstream>
#include <iomanip>
#include <future>
#include <chrono>

class GamePlayViewController: public BaseViewController {
public:
    GamePlayViewController(PlayerType player1Type, PlayerType player2Type, int X, int Y, int N);
    ~GamePlayViewController() noexcept override;
    void render() override;
    void checkEvent() override;
private:
    const int boardPaddingX = 4, boardPaddingY = 2;
    const int sidebarWidth = 16;

    const int aiTimeBudget = 10000;

    int sizeX, sizeY;
    Pos cursorPosition = Pos{0, 0};
    Game game;
    Chess currentPlayer = Player1;
    std::unique_ptr<BasePlayer> player1, player2;
    std::atomic<std::optional<Pos>> currentPlacedPosition = std::atomic(std::optional<Pos>(std::nullopt));
    std::optional<Chess> winner = std::nullopt;
    bool drawGame = false;

    // Because std::async will block if its future is not moved,
    // we need to store it although we never use it. C++ is weird
    [[maybe_unused]] std::future<void> aiPlayerFuture;

    std::chrono::time_point<std::chrono::system_clock> startTime;
    std::chrono::seconds currentElapsedSeconds = std::chrono::seconds(0);

    WINDOW *boardWindow = nullptr, *sidebarWindow = nullptr;

    constexpr bool isBoardFinal() const noexcept {
        return drawGame || winner.has_value();
    }

    constexpr BasePlayer * getCurrentPlayerObject() const noexcept {
        if (currentPlayer == Player1)
            return player1.get();
        else
            return player2.get();
    }

    constexpr const wchar_t * getChessText(Chess chess) const noexcept {
        switch (chess) {
            case Empty:
                return L" ";
            case Player1:
                return L"●";
            case Player2:
                return L"○";
        }
    }

    const std::wstring getPlayerDescription(Chess player) const noexcept {
        if (player == Player1)
            return std::wstring(L"Player 1 ") + getChessText(player);
        else
            return std::wstring(L"Player 2 ") + getChessText(player);
    };

    void drawBoard(WINDOW *window, const Board &board, Pos pos) noexcept;
    void updateChess(WINDOW *window, Pos pos, Chess chess, bool highlight = false, bool refresh = false) noexcept;
    void updateInformation() noexcept;
    void updateTimer() noexcept;

    void requestNextMove(std::optional<Pos> lastPlacedPosition) noexcept;
    bool tryMoveCursor(Pos newPosition) noexcept;
    void playGame() noexcept;

    inline void resetTimer() noexcept {
        startTime = std::chrono::system_clock::now();
        currentElapsedSeconds = std::chrono::seconds(0);
    }
    inline std::chrono::seconds getElapsedTime() const noexcept {
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - startTime);
    }

    void handleKeyboardEvent(int key) noexcept;
    void release() noexcept;
};

#endif //CONNECT4_GAME_PLAY_H
