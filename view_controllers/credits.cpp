#include "credits.h"

void CreditsController::render() {
    handleDialogEvent(false); // Remove any dialogs

    int x, y;
    std::tie(x, y) = UI::getWindowSize();

    showDialog("Connect4 by lin_toto");
    mvprintw(y - 1, 0, "Press <ENTER> to return");
}

void CreditsController::checkEvent() {
    timeout(0);
    int c = getch();
    if (c == 10) // Enter
        if (handleDialogEvent()) {
            UI::getInstance()->stateTransition(StateMainMenu);
        }
}

CreditsController::~CreditsController() noexcept {
    handleDialogEvent(false); // Remove any dialogs
}