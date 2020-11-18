#ifndef CONNECT4_GAME_PLAY_H
#define CONNECT4_GAME_PLAY_H

#include "base_view_controller.h"
#include "players/base_player.h"

class GamePlayViewController: public BaseViewController {
public:
    GamePlayViewController(PlayerType player1Type, PlayerType player2Type, int X, int Y, int N):
            player1(player1Type), player2(player2Type), sizeX(X), sizeY(Y), connectN(N) {}
    ~GamePlayViewController() noexcept override { release(); }
    void render() override;
    void checkEvent() override;
private:
    PlayerType player1, player2;
    int sizeX, sizeY, connectN;

    void handleKeyboardEvent(int key) noexcept;
    void release() noexcept;
};

#endif //CONNECT4_GAME_PLAY_H
