//
// Created by Toto Lin on 2020/11/18.
//

#include "base.h"

std::pair<MENU *, ITEM **> BaseViewController::makeMenu(const std::pair<std::string, State> choices[], int n) noexcept {
    ITEM **myItems = static_cast<ITEM **>(calloc(n + 1, sizeof(ITEM *)));
    for (int i = 0; i < n; i++) {
        myItems[i] = new_item(choices[i].first.c_str(), "");
        set_item_userptr(myItems[i], const_cast<void *>(reinterpret_cast<const void *>(&choices[i].second)));
    }
    myItems[n] = nullptr;

    MENU *myMenu = new_menu(myItems);
    set_menu_mark(myMenu, " * ");
    return std::make_pair(myMenu, myItems);
}