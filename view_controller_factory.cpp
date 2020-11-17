#include "view_controller_factory.h"

std::unique_ptr<BaseViewController> ViewControllerFactory::create(State state) {
    switch(state) {
        case StateMainMenu:
            return std::make_unique<MainMenuViewController>();
        case StateGameOptions:
            return std::make_unique<GameOptionsViewController>();
        case StateGame:
            break;
        case StateCredits:
            break;
        default:
            throw std::runtime_error("Unrecognized state");
    }
}