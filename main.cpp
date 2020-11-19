#include "ui.h"
#include <csignal>

UI *ui = nullptr;

int main() {
    ui = UI::getInstance();
    signal(SIGWINCH, UI::onWindowResize);
    signal(SIGINT, [](int) {
        ui->exitProgram();
    });
    ui->eventLoop();

    delete ui;
    return 0;
}
