#ifndef CONNECT4_MAIN_MENU_H
#define CONNECT4_MAIN_MENU_H

#include "base_view_controller.h"
#include "ui.h"
#include <chrono>

class MainMenuViewController: public BaseViewController {
public:
    MainMenuViewController();
    ~MainMenuViewController() noexcept override;
    void render() override;
    void checkEvent() override;
private:
    static const int menuChoicesCount = 3;
    const std::pair<std::string, State> menuChoices[menuChoicesCount] = {
            {"Play Game", State::StateGameOptions},
            {"Credits", State::StateCredits},
            {"Exit", State::Exit}
    };
    static const int menuHeight = 5, menuWidth = 15;

    const std::chrono::milliseconds logoUpdatePeriod = std::chrono::milliseconds(100);
    static const int logoLineCount = 6;
    const std::string logo[logoLineCount] = {
            R"(   ____                                  _    _  _   )",
            R"(  / ___| ___   _ __   _ __    ___   ___ | |_ | || |  )",
            R"( | |    / _ \ | '_ \ | '_ \  / _ \ / __|| __|| || |_ )",
            R"( | |___| (_) || | | || | | ||  __/| (__ | |_ |__   _|)",
            R"(  \____|\___/ |_| |_||_| |_| \___| \___| \__|   |_|  )",
            R"(                                                     )"
    };
    static const int logoSidePadding = 2;
    unsigned char currentLogoColorCode = 0xC0;
    std::chrono::time_point<std::chrono::system_clock> lastLogoUpdateTime;

    MENU *mainMenu = nullptr;
    ITEM **mainMenuItems = nullptr;
    WINDOW *logoWindow = nullptr, *menuWindow = nullptr, *menuSubWindow = nullptr;

    void paintLogo() noexcept;
    inline bool shouldLogoUpdate() const noexcept {
        return std::chrono::system_clock::now() - lastLogoUpdateTime > logoUpdatePeriod;
    }

    void handleKeyboardEvent(int key) noexcept;
    void release() noexcept;
};

#endif //CONNECT4_MAIN_MENU_H
