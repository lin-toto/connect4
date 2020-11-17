#ifndef CONNECT4_GAME_OPTIONS_H
#define CONNECT4_GAME_OPTIONS_H

#include "base.h"
#include "ui.h"

class GameOptionsViewController: public BaseViewController {
public:
    ~GameOptionsViewController() noexcept override { release(); }
    void render() override;
    void checkEvent() override;
private:
    enum Panel: char {
        Player1Radio = 0,
        Player2Radio = 1,
        AdvancedOptions = 2,
        BottomMenu = 3
    };
    Panel currentPanel = BottomMenu;

    const std::string windowTitle = "Game Options";
    const int formBoxHeight = 10, formBoxWidth = 16, formBoxPadding = 6, formBoxTopPadding = 2;
    static const int advancedOptionCount = 3;
    const std::string advancedOptionLabel[advancedOptionCount] = { "Rows Y", "Columns X", "Connect N" };
    static const int playerTypesCount = 4;
    const char *playerTypes[playerTypesCount] = { "Human", "MCTS Agent", "QLearn Agent", "GAN-CNN Agent" };

    const int menuBoxWidth = 21;
    static const int menuChoicesCount = 3;
    const std::pair<std::string, State> menuChoices[menuChoicesCount] = {
            {"Start Game", State::StateGame},
            {"Back", State::StateMainMenu}
    };

    CDKRADIO *player1TypeRadio = nullptr, *player2TypeRadio = nullptr;
    WINDOW *popupWindow = nullptr, *advancedOptionWindow = nullptr, *bottomMenuWindow = nullptr;
    CDKSCREEN *popupScreen = nullptr;
    FIELD *advancedOptionFields[advancedOptionCount + 1] = { nullptr, nullptr, nullptr, nullptr };
    FORM *advancedOptionForm = nullptr;
    MENU *bottomMenu = nullptr;
    ITEM **bottomMenuItems = nullptr;

    void handleKeyboardEvent(int key);
    bool handleRadioKeyboardEvent(CDKRADIO *radio, int key);
    bool handleFormKeyboardEvent(FORM *form, int key);
    bool handleMenuKeyboardEvent(MENU *menu, int key);
    void release() noexcept;
};


#endif //CONNECT4_GAME_OPTIONS_H
