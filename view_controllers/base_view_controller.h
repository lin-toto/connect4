#ifndef CONNECT4_BASE_VIEW_CONTROLLER_H
#define CONNECT4_BASE_VIEW_CONTROLLER_H

#define NCURSES_WIDECHAR 1

#include <cdk.h>
#include <ncursesw/ncurses.h>
#include <ncursesw/menu.h>
#include <ncursesw/form.h>
#include <functional>
#include <string>
#include "ui_state.h"


class BaseViewController {
public:
    virtual void render() = 0;
    virtual void checkEvent() {}
    virtual ~BaseViewController() = default;
    BaseViewController() = default;
    BaseViewController(const BaseViewController &other) = delete;
    BaseViewController & operator=(const BaseViewController &other) = delete;
protected:
    [[nodiscard]] static std::pair<MENU *, ITEM **> makeMenu (
            const std::pair<std::string, State> choices[], int n) noexcept;
    CDKDIALOG * showDialog(const std::string& message);

    bool handleDialogEvent(bool rerender = true) noexcept;

private:
    CDKSCREEN *dialogScreen = nullptr;
    CDKDIALOG *currentDialog = nullptr;
};

#endif //CONNECT4_BASE_VIEW_CONTROLLER_H
