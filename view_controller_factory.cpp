#include "view_controller_factory.h"

std::unique_ptr<BaseViewController> ViewControllerFactory::create(State state) {
    switch(state) {
        case StateMainMenu:
            return std::make_unique<MainMenuViewController>();
        case StateGameOptions:
            return std::make_unique<GameOptionsViewController>();
        case StateGamePlay:
            throw std::runtime_error("State GamePlay needs additional parameters");
        case StateCredits:
            return std::make_unique<CreditsController>();
            break;
        default:
            throw std::runtime_error("Unrecognized state");
    }
}