//
// Created by Toto Lin on 2020/11/18.
//

#include "main_menu.h"

MainMenuViewController::MainMenuViewController() {
    init_pair(0xC0, COLOR_CYAN, COLOR_BLACK);
    init_pair(0xC1, COLOR_GREEN, COLOR_BLACK);
    init_pair(0xC2, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(0xC3, COLOR_RED, COLOR_BLACK);
    init_pair(0xC4, COLOR_YELLOW, COLOR_BLACK);
}

void MainMenuViewController::render() {
    release();

    int x, y;
    std::tie(x, y) = UI::getWindowSize();

    int logoLength = logo[0].length();
    if (x >= logoLength + logoSidePadding * 2 && y >= logoLineCount + menuHeight) {
        logoWindow = newwin(logoLineCount, logoLength, (y - menuHeight - logoLineCount) / 2, (x - logoLength) / 2);
        paintLogo();
    }

    std::tie(mainMenu, mainMenuItems) = makeMenu(menuChoices, menuChoicesCount);
    menuWindow = newwin(menuHeight, menuWidth,
                        (y - menuHeight + (logoWindow == nullptr ? 0 : logoLineCount)) / 2,
                        (x - menuWidth) / 2);
    keypad(menuWindow, true);
    set_menu_win(mainMenu, menuWindow);

    if (logoWindow == nullptr) {
        mvwprintw(menuWindow, 0, 0, "Connect4");
    }

    menuSubWindow = derwin(menuWindow, menuHeight - 2, menuWidth, 1, 0);
    set_menu_sub(mainMenu, menuSubWindow);

    post_menu(mainMenu);
    wrefresh(menuWindow);
    mvprintw(y - 1, 0, "Press <UP>/<DOWN> to navigate, <ENTER> to select option");
}

void MainMenuViewController::paintLogo() noexcept {
    if (logoWindow != nullptr) {
        wattron(logoWindow, COLOR_PAIR(currentLogoColorCode));
        for (int i = 0; i < logoLineCount; i++)
            mvwprintw(logoWindow, i, 0, logo[i].c_str());

        wattroff(logoWindow, COLOR_PAIR(currentLogoColorCode));
        wrefresh(logoWindow);

        currentLogoColorCode++;
        if (currentLogoColorCode > 0xC4) currentLogoColorCode = 0xC0;
        lastLogoUpdateTime = std::chrono::system_clock::now();
    }
}

void MainMenuViewController::checkEvent() {
    if (menuWindow != nullptr) {
        wtimeout(menuWindow, 0);
        int c = wgetch(menuWindow);
        if (c > 0) handleKeyboardEvent(c);
    }

    if (logoWindow != nullptr) {
        if (shouldLogoUpdate()) paintLogo();
    }
}

void MainMenuViewController::handleKeyboardEvent(int key) noexcept {
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
            case 10: // ENTER
                auto *ptr = reinterpret_cast<State *>(item_userptr(current_item(mainMenu)));
                UI::getInstance()->stateTransition(*ptr);
                return;
        }

        wrefresh(menuWindow);
        wrefresh(logoWindow);
    }
}

void MainMenuViewController::release() noexcept {
    if (logoWindow != nullptr) {
        delwin(logoWindow);
        logoWindow = nullptr;
    }

    if (mainMenu != nullptr) {
        unpost_menu(mainMenu);
        free_menu(mainMenu);
        mainMenu = nullptr;

        for (int i = 0; i < menuChoicesCount; i++)
            free_item(mainMenuItems[i]);

        free(mainMenuItems);
        mainMenuItems = nullptr;
    }

    if (menuWindow != nullptr) {
        delwin(menuWindow);
        menuWindow = nullptr;
    }

    if (menuSubWindow != nullptr) {
        delwin(menuSubWindow);
        menuSubWindow = nullptr;
    }
}

MainMenuViewController::~MainMenuViewController() noexcept {
    free_pair(0xC0);
    free_pair(0xC1);
    free_pair(0xC2);
    free_pair(0xC3);
    free_pair(0xC4);
    release();
}
