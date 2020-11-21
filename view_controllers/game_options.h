#ifndef CONNECT4_GAME_OPTIONS_H
#define CONNECT4_GAME_OPTIONS_H

#include "base_view_controller.h"
#include "players/base_player.h"
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

    struct OptionData {
        PlayerType player1Type = Human, player2Type = Human;
        int Y = 6, X = 7, N = 4;
    };

    const int panelCount = 4;
    Panel currentPanel = BottomMenu;

    const std::string windowTitle = "Game Options";
    const int formBoxHeight = 10, formBoxWidth = 16, formBoxPadding = 6, formBoxTopPadding = 2;
    static const int advancedOptionCount = 3;
    const std::string advancedOptionLabel[advancedOptionCount] = { "Rows Y", "Columns X", "Connect N" };
    static const int playerTypesCount = 3;
    const char *playerTypes[playerTypesCount] = { "Human", "MCTS Agent", "QLearn Agent" };

    const int menuBoxWidth = 21;
    static const int menuChoicesCount = 3;
    const std::pair<std::string, State> menuChoices[menuChoicesCount] = {
            {"Start Game", State::StateGamePlay},
            {"Back", State::StateMainMenu}
    };

    OptionData autoSaveOptionData;

    CDKRADIO *player1TypeRadio = nullptr, *player2TypeRadio = nullptr;
    WINDOW *popupWindow = nullptr, *advancedOptionWindow = nullptr, *bottomMenuWindow = nullptr;
    CDKSCREEN *popupScreen = nullptr;
    FIELD *advancedOptionFields[advancedOptionCount + 1] = { nullptr, nullptr, nullptr, nullptr };
    FORM *advancedOptionForm = nullptr;
    MENU *bottomMenu = nullptr;
    ITEM **bottomMenuItems = nullptr;

    OptionData getSelectedOptions() const noexcept;
    void restoreAutoSavedOptions() noexcept;

    void updatePanelFocus() noexcept;

    void handleKeyboardEvent(int key) noexcept;
    bool handleRadioKeyboardEvent(CDKRADIO *radio, int key) noexcept;
    bool handleFormKeyboardEvent(FORM *form, FIELD **fields, int key) noexcept;
    bool handleMenuKeyboardEvent(MENU *menu, int key) noexcept;
    void release() noexcept;
};


#endif //CONNECT4_GAME_OPTIONS_H
