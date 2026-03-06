//
// Created by natasa on 3/6/26.
//

#include "../include/GUIController.h"

#include "engine/graphics/BloomController.h"
#include "engine/graphics/GraphicsController.hpp"
#include "engine/platform/PlatformController.hpp"

#include <imgui.h>

namespace app {

std::string_view GUIController::name() const {
    return "app::GUIControler";
}
void GUIController::initialize() {
    set_enable(false);
}

void GUIController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KeyId::KEY_K).state() == engine::platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
    }
}

void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    auto bloomController = engine::core::Controller::get<engine::graphics::BloomController>();


    graphics->begin_gui();

    ImGui::Begin("INFO");

    if (ImGui::CollapsingHeader("Keys Info", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("RIGHT - Agent moves right");
        ImGui::Text("LEFT - Agent moves left");
        ImGui::Text("F - BlinPhong light enabled");
        ImGui::Text("B - Bloom Effect");
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Camera Info", ImGuiTreeNodeFlags_DefaultOpen)) {
        const auto &c = *camera;
        ImGui::Text("Camera position: (%.2f, %.2f, %.2f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%.2f, %.2f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%.2f, %.2f, %.2f)", c.Front.x, c.Front.y, c.Front.z);
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Bloom Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat("Bloom Intensity", &bloomController->bloom_intensity, 0.1f, 0.0f, 5.0f);
        ImGui::DragFloat("Exposure", &bloomController->exposure, 0.1f, 0.1f, 3.0f);
        ImGui::DragInt("Passes", &bloomController->bloom_passes, 1, 0, 20);
    }

    ImGui::End();

    graphics->end_gui();
}

}// namespace app