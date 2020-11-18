#include <iostream>

#include "game.h"
#include "board.h"
#include "players/mcts_player.h"
#include "players/qlearn_player.h"
#include "ui.h"
#include <optional>
#include <csignal>

UI *ui = nullptr;

int main() {
    ui = UI::getInstance();
    signal(SIGWINCH, UI::onWindowResize);
    ui->eventLoop();

    delete ui;
    return 0;
}
