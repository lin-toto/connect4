#ifndef CONNECT4_BASE_VIEW_CONTROLLER_H
#define CONNECT4_BASE_VIEW_CONTROLLER_H

#include <cdk.h>
#include <curses.h>
#include <menu.h>
#include <form.h>
#include <functional>
#include <string>
#include "ui_state.h"


class BaseViewController {
public:
    virtual void render() = 0;
    virtual void checkEvent() {}
    virtual ~BaseViewController() = default;
protected:
    [[nodiscard]] static std::pair<MENU *, ITEM **> makeMenu (
            const std::pair<std::string, State> choices[], int n) noexcept;
};

#endif //CONNECT4_BASE_VIEW_CONTROLLER_H
