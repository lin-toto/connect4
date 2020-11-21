#include "game_options.h"

void GameOptionsViewController::render() {
    release();

    int x, y;
    std::tie(x, y) = UI::getWindowSize();

    if (x < formBoxWidth * 3 + formBoxPadding * 3 || y < formBoxHeight + 4) {
        mvprintw(1, 2, "Window too small");
        mvprintw(2, 2, "Please enlarge terminal");
        return;
    }

    popupWindow = newwin(y, x, 0, 0);
    keypad(popupWindow, true);
    box(popupWindow, 0, 0);
    if (static_cast<unsigned int>(x) >= windowTitle.length()) {
        mvwprintw(popupWindow, 0, (x - static_cast<int>(windowTitle.length())) / 2, windowTitle.c_str());
    }
    mvwprintw(popupWindow, y - 2, 2, "Press Arrow Keys/<TAB> to navigate, <ENTER> to select option");
    wrefresh(popupWindow);

    popupScreen = initCDKScreen(popupWindow);

    player1TypeRadio = newCDKRadio(popupScreen, formBoxPadding / 2, formBoxTopPadding, NONE,
                                   formBoxHeight, formBoxWidth, "Player 1",
                                   const_cast<char **>(playerTypes), playerTypesCount,
                                   '*', 0, 0, false, false);
    player2TypeRadio = newCDKRadio(popupScreen, formBoxPadding * 3 / 2 + formBoxWidth, formBoxTopPadding, NONE,
                                   formBoxHeight, formBoxWidth, "Player 2",
                                   const_cast<char **>(playerTypes), playerTypesCount,
                                   '*', 0, 0, false, false);
    drawCDKRadio(player1TypeRadio, false);
    drawCDKRadio(player2TypeRadio, false);

    advancedOptionWindow = derwin(popupWindow, formBoxHeight, formBoxWidth,
                                  formBoxTopPadding, formBoxPadding * 5 / 2 + formBoxWidth * 2);
    for (int i = 0; i < advancedOptionCount; i++) {
        advancedOptionFields[i] = new_field(1, formBoxWidth / 3, i + 1, formBoxWidth * 2 / 3, 0, 0);
        set_field_back(advancedOptionFields[i], A_UNDERLINE);
        field_opts_off(advancedOptionFields[i], O_AUTOSKIP);
        field_opts_off(advancedOptionFields[i], O_NULLOK);
    }

    advancedOptionForm = new_form(advancedOptionFields);
    set_form_win(advancedOptionForm, popupWindow);
    set_form_sub(advancedOptionForm, advancedOptionWindow);

    restoreAutoSavedOptions();

    post_form(advancedOptionForm);
    form_driver(advancedOptionForm, REQ_END_LINE);

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

    updatePanelFocus();
    // Do not refresh popupWindow here, handled by updatePanelFocus.
}

void GameOptionsViewController::checkEvent() {
    if (popupWindow != nullptr) {
        wtimeout(popupWindow, 0);
        int c = wgetch(popupWindow);
        if (c > 0) handleKeyboardEvent(c);
    }
}

void GameOptionsViewController::handleKeyboardEvent(int key) noexcept {
    if (handleDialogEvent())
        return;

    if (popupWindow != nullptr && advancedOptionWindow != nullptr
        && player1TypeRadio != nullptr && player2TypeRadio != nullptr
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
                eventHandledByPanel = handleFormKeyboardEvent(advancedOptionForm, advancedOptionFields, key);
                break;
            case BottomMenu:
                eventHandledByPanel = handleMenuKeyboardEvent(bottomMenu, key);
                break;
        }

        if (!eventHandledByPanel) {
            // Switch focus between panels.

            Panel previousPanel = currentPanel;
            switch (key) {
                default:
                    break;
                case KEY_TAB:
                    currentPanel = static_cast<Panel>((static_cast<char>(currentPanel) + 1) % panelCount);
                    break;
                case KEY_UP:
                    if (currentPanel == BottomMenu)
                        currentPanel = Player2Radio;
                    break;
                case KEY_DOWN:
                    currentPanel = BottomMenu;
                    break;
                case KEY_LEFT:
                case KEY_RIGHT:
                    currentPanel = static_cast<Panel>(
                            (static_cast<char>(currentPanel) + (key == KEY_LEFT ? -1 : 1)
                            + (panelCount - 1)) % (panelCount - 1));
                    break;
            }

            if (previousPanel != currentPanel) updatePanelFocus();
        } else if (currentPanel != BottomMenu) {
            // Assume some option was changed, store it in autosave
            autoSaveOptionData = getSelectedOptions();
        }
    }
}

bool GameOptionsViewController::handleRadioKeyboardEvent(CDKRADIO *radio, int key) noexcept { // NOLINT
    int itemIndex = getCDKRadioCurrentItem(radio);
    switch (key) {
        case KEY_DOWN:
            if (itemIndex == playerTypesCount - 1)
                return false;
        case KEY_UP:
        case 10: // Enter
        case 32: // Space
            // CDK won't recognize enter key, convert it to space
            if (key == 10) key = 32;
            injectCDKRadio(radio, key);
            return true;
        default:
            return false;
    }
}

bool GameOptionsViewController::handleFormKeyboardEvent(FORM *form, FIELD **fields, int key) noexcept { // NOLINT
    switch (key) {
        case KEY_DOWN:
            if (current_field(form) == fields[advancedOptionCount - 1])
                return false;

            form_driver(form, REQ_NEXT_FIELD);
            form_driver(form, REQ_END_LINE);
            pos_form_cursor(form);
            return true;
        case KEY_UP:
            form_driver(form, REQ_PREV_FIELD);
            form_driver(form, REQ_END_LINE);
            pos_form_cursor(form);
            return true;
        case KEY_BACKSPACE: case 127: // Backspace
            form_driver(form, REQ_DEL_PREV);
            return true;
        case 48: case 49: case 50: case 51: case 52:
        case 53: case 54: case 55: case 56: case 57:
            // Number, treat as form input
            form_driver(form, key);
            form_driver(form, REQ_END_LINE);
            return true;
        default:
            return false;
    }
}

bool GameOptionsViewController::handleMenuKeyboardEvent(MENU *menu, int key) noexcept {
    switch (key) {
        default:
            return false;
        case KEY_LEFT:
            menu_driver(menu, REQ_LEFT_ITEM);
            return true;
        case KEY_RIGHT:
            menu_driver(menu, REQ_RIGHT_ITEM);
            return true;
        case 10: //Enter
            State state = *reinterpret_cast<State *>(item_userptr(current_item(menu)));
            if (state != StateGamePlay) {
                UI::getInstance()->stateTransition(state);
            } else {
                auto option = getSelectedOptions();
                if (option.Y < 0 || option.Y > 50)
                    showDialog("Invalid Y value! (0-50)");
                else if (option.X < 0 || option.X > 50)
                    showDialog("Invalid X value! (0-50)");
                else if (option.N < 0 || option.N > 50)
                    showDialog("Invalid N value! (0-50)");
                else
                    UI::getInstance()->beginGamePlay(option.player1Type, option.player2Type,
                                                     option.Y, option.X, option.N);
            }
            return true;
    }
}

void GameOptionsViewController::updatePanelFocus() noexcept {
    if (currentPanel == Player1Radio)
        setCDKRadioHighlight(player1TypeRadio, A_REVERSE);
    else
        setCDKRadioHighlight(player1TypeRadio, 0);

    if (currentPanel == Player2Radio)
        setCDKRadioHighlight(player2TypeRadio, A_REVERSE);
    else
        setCDKRadioHighlight(player2TypeRadio, 0);

    drawCDKRadio(player1TypeRadio, false);
    drawCDKRadio(player2TypeRadio, false);

    if (currentPanel == BottomMenu)
        set_menu_fore(bottomMenu, A_REVERSE);
    else
        set_menu_fore(bottomMenu, A_NORMAL);

    wrefresh(popupWindow);

    if (currentPanel == AdvancedOptions) {
        curs_set(1);
        pos_form_cursor(advancedOptionForm);
    } else curs_set(0);
}

GameOptionsViewController::OptionData GameOptionsViewController::getSelectedOptions() const noexcept {
    OptionData optionData{};
    optionData.player1Type = static_cast<PlayerType>(getCDKRadioSelectedItem(player1TypeRadio));
    optionData.player2Type = static_cast<PlayerType>(getCDKRadioSelectedItem(player2TypeRadio));
    optionData.Y = atoi(field_buffer(advancedOptionFields[0], 0));
    optionData.X = atoi(field_buffer(advancedOptionFields[1], 0));
    optionData.N = atoi(field_buffer(advancedOptionFields[2], 0));

    return optionData;
}

void GameOptionsViewController::restoreAutoSavedOptions() noexcept {
    setCDKRadioSelectedItem(player1TypeRadio, static_cast<int>(autoSaveOptionData.player1Type));
    setCDKRadioSelectedItem(player2TypeRadio, static_cast<int>(autoSaveOptionData.player2Type));
    set_field_buffer(advancedOptionFields[0], 0, std::to_string(autoSaveOptionData.Y).c_str());
    set_field_buffer(advancedOptionFields[1], 0, std::to_string(autoSaveOptionData.X).c_str());
    set_field_buffer(advancedOptionFields[2], 0, std::to_string(autoSaveOptionData.N).c_str());
}

void GameOptionsViewController::release() noexcept {
    handleDialogEvent(false);

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