#include "game_options.h"

void GameOptionsViewController::render() {
    release();

    int x, y;
    std::tie(x, y) = UI::getInstance()->getWindowSize();

    if (x < formBoxWidth * 3 + formBoxPadding * 3 || y < formBoxHeight + 4) {
        mvprintw(1, 2, "Window too small");
        mvprintw(2, 2, "Please enlarge terminal");
        return;
    }

    popupWindow = newwin(y, x, 0, 0);
    keypad(popupWindow, true);
    box(popupWindow, 0, 0);
    if (x >= windowTitle.length()) {
        mvwprintw(popupWindow, 0, (x - static_cast<int>(windowTitle.length())) / 2, windowTitle.c_str());
    }
    mvwprintw(popupWindow, y - 2, 2, "Press Arrow Keys/<TAB> to navigate, <ENTER> to select option");
    wrefresh(popupWindow);

    popupScreen = initCDKScreen(popupWindow);

    player1TypeRadio = newCDKRadio(popupScreen, formBoxPadding / 2, formBoxTopPadding, NONE,
                                   formBoxHeight, formBoxWidth, "Player 1",
                                   const_cast<char **>(playerTypes), playerTypesCount,
                                   '*', 0, A_REVERSE, false, false);
    drawCDKRadio(player1TypeRadio, false);
    player2TypeRadio = newCDKRadio(popupScreen, formBoxPadding * 3 / 2 + formBoxWidth, formBoxTopPadding, NONE,
                                   formBoxHeight, formBoxWidth, "Player 2",
                                   const_cast<char **>(playerTypes), playerTypesCount,
                                   '*', 0, A_REVERSE, false, false);
    drawCDKRadio(player2TypeRadio, false);

    advancedOptionWindow = derwin(popupWindow, formBoxHeight, formBoxWidth,
                                  formBoxTopPadding, formBoxPadding * 5 / 2 + formBoxWidth * 2);
    for (int i = 0; i < advancedOptionCount; i++) {
        advancedOptionFields[i] = new_field(1, formBoxWidth / 3, i + 1, formBoxWidth * 2 / 3, 0, 0);
        set_field_back(advancedOptionFields[i], A_UNDERLINE);
        field_opts_off(advancedOptionFields[i], O_AUTOSKIP);
    }

    advancedOptionForm = new_form(advancedOptionFields);
    set_form_win(advancedOptionForm, popupWindow);
    set_form_sub(advancedOptionForm, advancedOptionWindow);

    post_form(advancedOptionForm);

    mvwprintw(advancedOptionWindow, 0, 0, "Advanced");
    for (int i = 0; i < advancedOptionCount; i++) {
        mvwprintw(advancedOptionWindow, i + 1, 0, (advancedOptionLabel[i] + ":").c_str());
    }

    mvwhline(popupWindow, y - 6, 1, ACS_HLINE, x - 2);

    bottomMenuWindow = derwin(popupWindow, 1, menuBoxWidth, y - 4, (x - menuBoxWidth) / 2);
    std::tie(bottomMenu, bottomMenuItems) = makeMenu(menuChoices, menuChoicesCount);
    set_menu_win(bottomMenu, popupWindow);
    set_menu_sub(bottomMenu, bottomMenuWindow);
    set_menu_format(bottomMenu, 1, 2);
    set_menu_mark(bottomMenu, "   ");
    post_menu(bottomMenu);

    wrefresh(bottomMenuWindow);
    wrefresh(popupWindow);
}

void GameOptionsViewController::checkEvent() {
    if (popupWindow != nullptr) {
        wtimeout(popupWindow, 0);
        if (int c = wgetch(popupWindow))
            handleKeyboardEvent(c);
    }
}

void GameOptionsViewController::handleKeyboardEvent(int key) {
    if (popupWindow != nullptr && player1TypeRadio != nullptr && player2TypeRadio != nullptr
        && advancedOptionForm != nullptr && bottomMenu != nullptr) {
        bool eventHandledByPanel = false;
        switch (currentPanel) {
            case Player1Radio:
                eventHandledByPanel = handleRadioKeyboardEvent(player1TypeRadio, key);
                break;
            case Player2Radio:
                eventHandledByPanel = handleRadioKeyboardEvent(player2TypeRadio, key);
                break;
            case AdvancedOptions:
                eventHandledByPanel = handleFormKeyboardEvent(advancedOptionForm, key);
                break;
            case BottomMenu:
                eventHandledByPanel = handleMenuKeyboardEvent(bottomMenu, key);
                break;
        }

        if (!eventHandledByPanel) {
            // Switch focus between panels.

            switch (key) {

            }
        }
    }
}

bool GameOptionsViewController::handleRadioKeyboardEvent(CDKRADIO *radio, int key) {

}

bool GameOptionsViewController::handleFormKeyboardEvent(FORM *form, int key) {

}

bool GameOptionsViewController::handleMenuKeyboardEvent(MENU *menu, int key) {

}

void GameOptionsViewController::release() noexcept {
    if (advancedOptionForm != nullptr) {
        unpost_form(advancedOptionForm);
        free_form(advancedOptionForm);
        advancedOptionForm = nullptr;

        for (int i = 0; i < advancedOptionCount; i++)
            free_field(advancedOptionFields[i]);

        std::fill(advancedOptionFields, advancedOptionFields + advancedOptionCount, nullptr);
    }

    if (bottomMenu != nullptr) {
        unpost_menu(bottomMenu);
        free_menu(bottomMenu);
        bottomMenu = nullptr;

        for (int i = 0; i < menuChoicesCount; i++)
            free_item(bottomMenuItems[i]);

        free(bottomMenuItems);
        bottomMenuItems = nullptr;
    }

    if (advancedOptionWindow != nullptr) {
        delwin(advancedOptionWindow);
        advancedOptionWindow = nullptr;
    }

    if (player1TypeRadio != nullptr) {
        destroyCDKRadio(player1TypeRadio);
        player1TypeRadio = nullptr;
    }

    if (player2TypeRadio != nullptr) {
        destroyCDKRadio(player2TypeRadio);
        player2TypeRadio = nullptr;
    }

    if (popupScreen != nullptr) {
        destroyCDKScreen(popupScreen);
        popupScreen = nullptr;
    }

    if (popupWindow != nullptr) {
        delwin(popupWindow);
        popupWindow = nullptr;
    }
}