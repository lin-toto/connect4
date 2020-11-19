#include "ui.h"

UI *UI::instance = nullptr;

UI::UI() noexcept {
    // Init ncurses library
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    resize();
    currentViewController = ViewControllerFactory::create(currentState);
}

UI::~UI() noexcept {
    endwin();
}

void UI::eventLoop() {
    while (!exitFlag) {
        currentViewController->checkEvent();

        if (rerenderFlag) {
            rerenderFlag = false;
            render();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void UI::render() {
    resize();
    clear();
    refresh();
    currentViewController->render();
    refresh();
}

std::pair<int, int> UI::getWindowSize() noexcept {
    int x, y;
    getmaxyx(stdscr, y, x);
    return std::make_pair(x, y);
}

void UI::resize() noexcept {
    std::tie(sizeX, sizeY) = getWindowSize();
}

void UI::onWindowResize(int) noexcept {
    struct winsize size{};

    if (ioctl(fileno(stdout), TIOCGWINSZ, &size) == 0) { // NOLINT
        resize_term(size.ws_row, size.ws_col);
    }
    UI::getInstance()->rerenderOnNextCycle();
}

void UI::stateTransition(State state) {
    if (state == Exit) {
        exitProgram();
    } else {
        currentState = state;
        currentViewController = ViewControllerFactory::create(currentState);
        rerenderOnNextCycle();
    }
}

void UI::beginGamePlay(PlayerType player1Type, PlayerType player2Type, int Y, int X, int N) {
    currentState = StateGamePlay;
    currentViewController = std::make_unique<GamePlayViewController>(player1Type, player2Type, X, Y, N);
    rerenderOnNextCycle();
}