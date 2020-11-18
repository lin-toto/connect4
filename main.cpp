#include "ui.h"
#include <csignal>

UI *ui = nullptr;

int main() {
    ui = UI::getInstance();
    signal(SIGWINCH, UI::onWindowResize);
    ui->eventLoop();

    delete ui;
    return 0;
}
