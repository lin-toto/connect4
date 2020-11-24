#include "ui.h"
#include <csignal>

UI *ui = nullptr;

int main() {
    ui = UI::getInstance();
#ifndef _WIN32 // Doesn't work on windows :(
    signal(SIGWINCH, UI::onWindowResize);
    signal(SIGINT, [](int) {
        ui->exitProgram();
    });
#endif
    ui->eventLoop();

    delete ui;
    return 0;
}
