#ifndef CONNECT4_UI_H
#define CONNECT4_UI_H

#include <curses.h>
#include <menu.h>
#include <functional>
#include <string>
#include <clocale>
#include <unistd.h>
#include <sys/ttycom.h>
#include <sys/ioctl.h>

enum State: char {
    StateMainMenu,
    StateGameOptions,
    StateGame,
    StateCredits,
    Exit
};

class ViewController {
public:
    virtual void render() = 0;
    virtual void checkEvent() {}
protected:
    [[nodiscard]] static std::pair<MENU *, ITEM **> makeMenu (const std::pair<std::string, State> choices[], int n) noexcept;
    void checkKeyboardEvent(WINDOW *window) noexcept {
        wtimeout(window, 0);
        if (int c = wgetch(window))
            handleKeyboardEvent(c);
    }
    virtual void handleKeyboardEvent(int key) {}
};

class UI {
public:
    void eventLoop();
    void render();
    static std::pair<int, int> getWindowSize() noexcept;
    constexpr void exitProgram() noexcept { exitFlag = true; }

    static UI * getInstance () noexcept {
        if (instance == nullptr)
            instance = new UI;

        return instance;
    }

    static void onWindowResize(int) noexcept;
    void stateTransition(State state);

    ~UI() noexcept;
    UI(const UI &) = delete;
    UI & operator=(const UI &) = delete;
private:
    static UI *instance;
    UI() noexcept;

    const State firstState = StateMainMenu;
    State currentState = firstState;

    int sizeX = 0, sizeY = 0;
    std::unique_ptr<ViewController> currentViewController;
    bool exitFlag = false, windowResized = false;

    void resize() noexcept;
};

class MainMenuViewController: public ViewController {
public:
    void render() override;
private:
    static const int menuChoicesCount = 3;
    const std::pair<std::string, State> menuChoices[menuChoicesCount] = {
        {"Play Game", State::StateGameOptions},
        {"Credits", State::StateCredits},
        {"Exit", State::Exit}
    };

    MENU *mainMenu = nullptr;
    ITEM **mainMenuItems = nullptr;
    WINDOW *logoWindow = nullptr, *menuWindow = nullptr, *menuSubWindow = nullptr;

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

    static const int menuHeight = 5;

    void checkEvent() override { if (menuWindow != nullptr) checkKeyboardEvent(menuWindow); }
    void handleKeyboardEvent(int key) override;
};

class ViewControllerFactory {
public:
    static std::unique_ptr<ViewController> create(State state) {
        switch(state) {
            case StateMainMenu:
                return std::make_unique<MainMenuViewController>();
            case StateGameOptions:
                break;
            case StateGame:
                break;
            case StateCredits:
                break;
            default:
                throw std::runtime_error("Unrecognized state");
        }
    }
};

#endif //CONNECT4_UI_H
