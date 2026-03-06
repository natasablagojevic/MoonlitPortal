//
// Created by natasa on 3/6/26.
//

#ifndef MATF_RG_PROJECT_GUICONTROLLER_H
#define MATF_RG_PROJECT_GUICONTROLLER_H
#include "engine/core/Controller.hpp"

namespace app {

class GUIController : public engine::core::Controller {
public:
std::string_view name() const override;

private:
    void initialize() override;
    void poll_events() override;
    void draw() override;
};

}// namespace app

#endif//MATF_RG_PROJECT_GUICONTROLLER_H
