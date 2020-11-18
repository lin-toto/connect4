#ifndef CONNECT4_CREDITS_H
#define CONNECT4_CREDITS_H

#include "base_view_controller.h"
#include "ui.h"

class CreditsController: public BaseViewController {
public:
    void render() override;
    void checkEvent() override;
    ~CreditsController() noexcept;
};

#endif //CONNECT4_CREDITS_H
