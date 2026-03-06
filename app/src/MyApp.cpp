//
// Created by natasa on 2/22/26.
//

#include <../include/MyApp.h>

#include "MainController.h"
#include <engine/graphics/BloomController.h>

#include <spdlog/spdlog.h>

namespace app {

void MyApp::app_setup() {
    spdlog::info("App setup completed...");

    auto mainController = register_controller<app::MainController>();
    auto bloomControler = register_controller<engine::graphics::BloomController>();

    mainController->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    bloomControler->after(mainController);

}

}// namespace app