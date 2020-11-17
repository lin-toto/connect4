#include "ui.h"

UI *UI::instance = nullptr;

UI::UI() noexcept {
    // Init ncurses library
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    resize();
    currentViewController = ViewControllerFactory::create(currentState);
}

UI::~UI() noexcept {
    endwin();
}

void UI::eventLoop() {
    while (!exitFlag) {
        currentViewController->checkEvent();

        if (windowResized) {
            windowResized = false;
            render();
        }

        usleep(1000);
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
    UI::getInstance()->windowResized = true;
}

void UI::stateTransition(State state) {
    if (state == Exit) {
        exitProgram();
    } else {
        currentState = state;
        currentViewController = ViewControllerFactory::create(state);
        render();
    }
}

#include <iostream>
std::pair<MENU *, ITEM **> ViewController::makeMenu(const std::pair<std::string, State> choices[], int n) noexcept {
    ITEM **myItems = static_cast<ITEM **>(calloc(n + 1, sizeof(ITEM *)));
    for (int i = 0; i < n; i++) {
        myItems[i] = new_item(choices[i].first.c_str(), "");
        set_item_userptr(myItems[i], const_cast<void *>(reinterpret_cast<const void *>(&choices[i].second)));
    }
    myItems[n] = nullptr;

    MENU *myMenu = new_menu(myItems);
    set_menu_mark(myMenu, " * ");
    return std::make_pair(myMenu, myItems);
}

void MainMenuViewController::render() {
    curs_set(0);

    if (logoWindow != nullptr) {
        delwin(logoWindow);
        logoWindow = nullptr;
    }

    if (mainMenu != nullptr) {
        unpost_menu(mainMenu);
        free_menu(mainMenu);
        for (int i = 0; i < menuChoicesCount; i++)
            free_item(mainMenuItems[i]);

        free(mainMenuItems);
    }

    if (menuWindow != nullptr) {
        delwin(menuWindow);
        menuWindow = nullptr;
    }

    if (menuSubWindow != nullptr) {
        delwin(menuSubWindow);
        menuSubWindow = nullptr;
    }

    int x, y;
    std::tie(x, y) = UI::getInstance()->getWindowSize();

    int logoLength = logo[0].length();
    if (x >= logoLength + logoSidePadding * 2 && y >= logoLineCount + menuHeight) {
        logoWindow = newwin(logoLineCount, logoLength, (y - menuHeight - logoLineCount) / 2, (x - logoLength) / 2);
        for (int i = 0; i < logoLineCount; i++)
            mvwprintw(logoWindow, i, 0, logo[i].c_str());
        wrefresh(logoWindow);
    }

    std::tie(mainMenu, mainMenuItems) = makeMenu(menuChoices, menuChoicesCount);
    menuWindow = newwin(menuHeight, 15,
                        (y - menuHeight + (logoWindow == nullptr ? 0 : logoLineCount)) / 2,
                        (x - 10) / 2);
    keypad(menuWindow, true);
    set_menu_win(mainMenu, menuWindow);

    if (logoWindow == nullptr) {
        mvwprintw(menuWindow, 0, 0, "Connect4");
    }

    menuSubWindow = derwin(menuWindow, menuHeight - 2, 15, 1, 0);
    set_menu_sub(mainMenu, menuSubWindow);

    post_menu(mainMenu);
    wrefresh(menuWindow);
    mvprintw(y - 1, 0, "Press <UP>/<DOWN> to navigate, <ENTER> to select option");
}

void MainMenuViewController::handleKeyboardEvent(int key) {
    if (menuWindow != nullptr && mainMenu != nullptr) {
        switch (key) {
            default:
                break;
            case KEY_DOWN:
                menu_driver(mainMenu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(mainMenu, REQ_UP_ITEM);
                break;
            case 10:
                auto *ptr = reinterpret_cast<State *>(item_userptr(current_item(mainMenu)));
                UI::getInstance()->stateTransition(*ptr);
                break;
        }

        wrefresh(menuWindow);
        wrefresh(logoWindow);
    }
}