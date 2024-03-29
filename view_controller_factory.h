#ifndef CONNECT4_VIEW_CONTROLLER_FACTORY_H
#define CONNECT4_VIEW_CONTROLLER_FACTORY_H

#include <functional>
#include <memory>
#include "view_controllers/base_view_controller.h"
#include "view_controllers/main_menu.h"
#include "view_controllers/game_options.h"
#include "view_controllers/credits.h"


class ViewControllerFactory {
public:
    static std::unique_ptr<BaseViewController> create(State state);
};

#endif //CONNECT4_VIEW_CONTROLLER_FACTORY_H
