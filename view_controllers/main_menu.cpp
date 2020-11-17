//
// Created by Toto Lin on 2020/11/18.
//

#include "main_menu.h"

void MainMenuViewController::render() {
    curs_set(0);
    release();

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

void MainMenuViewController::checkEvent() {
    if (menuWindow != nullptr) {
        wtimeout(menuWindow, 0);
        if (int c = wgetch(menuWindow))
            handleKeyboardEvent(c);
    }
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