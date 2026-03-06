//
// Created by natasa on 2/22/26.
//

#include "../include/MainController.h"

#include "engine/graphics/GraphicsController.hpp"
#include "engine/graphics/OpenGL.hpp"
#include "engine/platform/PlatformController.hpp"
#include "engine/resources/ResourcesController.hpp"
#include "imgui_impl_opengl3_loader.h"

#include <iostream>
#include <spdlog/spdlog.h>
namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
    void on_mouse_move(engine::platform::MousePosition position) override;
    void on_scroll(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    camera->rotate_camera(position.dx, position.dy);
}

void MainPlatformEventObserver::on_scroll(engine::platform::MousePosition position) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    auto mouse = engine::core::Controller::get<engine::platform::PlatformController>()->mouse();

    camera->zoom(mouse.scroll);
    graphics->perspective_params().FOV = glm::radians(camera->Zoom);
}
// -3.0, 3.0, -10.0
static glm::vec3 ship_position = glm::vec3(3.0f, 4.0f, -10.0f);
// static glm::vec3 ship_position = glm::vec3(-3.0f, 3.0f, -10.0f);
static glm::vec3 island_position = glm::vec3(0.0f, 0.0f, -10.0f);
static glm::vec3 agent_position = glm::vec3(0.0f, 0.0f, -10.0f); // y = sint
static glm::vec3 agent_position_origin = glm::vec3(0.0f, 0.0f, -10.0f);
// 3.0, 4.0, -10.0
static glm::vec3 moon_position = glm::vec3(-3.0f, 3.0f, -10.0f);
// static glm::vec3 moon_position = glm::vec3(3.0f, 4.0f, -10.0f);
static bool agent_alive = true;
static bool pressedB = false;
static bool BlinPhong = false;

// static glm::vec3 spotLightDiffuse_origin = glm::vec3(1.0f, 0.95f, 0.85f);

void app::MainController::initialize() {
    spdlog::info("MainController initialized....");

    // auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    // platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());

    this->bloomControler = get<engine::graphics::BloomController>();
    this->bloomControler->init();

    engine::graphics::OpenGL::enable_depth_testing();
}

bool app::MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }

    if (platform->key(engine::platform::KeyId::KEY_B).is_down()) {
        if (!pressedB) {
            // spotLightDiffuse_origin = glm::vec3(10.0f);
            this->bloomControler->toggle_bloom();
            pressedB = true;
        } else {
            // spotLightDiffuse_origin = glm::vec3(1.0f, 0.95f, 0.85f);
            pressedB = false;
        }
    }

    if (platform->key(engine::platform::KeyId::KEY_F).is_down()) {
        if (!BlinPhong) {
            BlinPhong = true;
        } else {
            BlinPhong = false;
        }
    }

    return true;
}

void MainController::update_camera() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto *camera = graphics->camera();

    float dt = platform->dt();

    if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
    }

    if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
    }

    if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
    }

    if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
    }

    // gore
    if (platform->key(engine::platform::KeyId::KEY_UP).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::UP, dt);
    }

    // dole
    if (platform->key(engine::platform::KeyId::KEY_DOWN).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt);
    }
}

void MainController::update_agent() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    float dt = platform->dt();
    float speed = 0.5f;

    if (platform->key(engine::platform::KeyId::KEY_RIGHT).is_down()) {
        agent_position.x += speed * dt;
    }

    if (platform->key(engine::platform::KeyId::KEY_LEFT).is_down()) {
        agent_position -= speed * dt;
    }

    if (agent_position.z <= -11.3f) {
        agent_alive = false;
    }

    if (platform->key(engine::platform::KeyId::KEY_SPACE).is_down()) {
        agent_alive = true;
        agent_position = agent_position_origin;
    }
}

void MainController::update() {
    update_camera();
    update_agent();
}

void app::MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw_ship() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model *ship = resources->model("space_ship");

    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();

    shader->set_mat4("P", graphics->projection_matrix());
    shader->set_mat4("V", graphics->camera()->view_matrix());

    float t = engine::core::Controller::get<engine::platform::PlatformController>()->frame_time().current;

    glm::mat4 M = glm::mat4(1.0f);
    M = glm::translate(M, ship_position);
    M = glm::scale(M, glm::vec3(0.085f));
    M = glm::rotate(M, glm::sin(t)*0.2f, glm::vec3(0.0f, 1.0f, 0.0f));

    shader->set_mat4("M", M);

    ship->draw(shader);
}

void MainController::draw_island() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model *island = resources->model("space_island");

    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();

    shader->set_mat4("P", graphics->projection_matrix());
    shader->set_mat4("V", graphics->camera()->view_matrix());

    float t = engine::core::Controller::get<engine::platform::PlatformController>()->frame_time().current;

    glm::mat4 M = glm::mat4(1.0f);
    M = glm::translate(M, island_position);
    M = glm::scale(M, glm::vec3(0.25f));
    M = glm::rotate(M, glm::abs(glm::sin(t * 0.5f)*0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
    shader->set_mat4("M", M);

    island->draw(shader);
}

void MainController::draw_agent() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    engine::resources::Model *agent = resources->model("agent");

    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();

    shader->set_mat4("P", graphics->projection_matrix());
    shader->set_mat4("V", graphics->camera()->view_matrix());

    float t = engine::core::Controller::get<engine::platform::PlatformController>()->frame_time().current;

    agent_position.y = glm::abs(glm::sin(t)) * 0.1f;

    glm::mat4 M = glm::mat4(1.0f);
    M = glm::translate(M, agent_position);
    M = glm::scale(M, glm::vec3(0.2f));
    shader->set_mat4("M", M);

    // std::cout << "Agent Position: " << agent_position.x << " ; " << agent_position.y << " ; " << agent_position.z << "\n";

    shader->set_float("material.shi", 64.0f);

    agent->draw(shader);
}

void MainController::draw_moon() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    engine::resources::Model *moon = resources->model("moon");

    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();

    shader->set_mat4("V", graphics->camera()->view_matrix());
    shader->set_mat4("P", graphics->projection_matrix());

    float t = engine::core::Controller::get<engine::platform::PlatformController>()->frame_time().current;

    glm::mat4 M = glm::mat4(1.0f);
    // M = glm::translate(M, moon_position);
    // M = glm::scale(M, glm::vec3(0.085f));
    // // M = glm::translate(M, glm::vec3(glm::cos(t)*50.0f, 0.0f, glm::sin(t)*50.0f));
    // M = glm::rotate(M, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // M = glm::rotate(M, glm::sin(t*0.5f), glm::vec3(0.0f, 1.0f, 0.0f));

    M = glm::mat4(1.0f);
    M = glm::translate(M, island_position);
    M = glm::rotate(M, t * 0.3f, glm::vec3(0.0f, 1.0f, 0.0f));
    M = glm::translate(M, glm::vec3(-3.0f, 2.5f, 0.0f));
    M = glm::rotate(M, t * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
    M = glm::scale(M, glm::vec3(0.085f));

    shader->set_mat4("M", M);

    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();

    shader->set_vec3("spotLight.pos", ship_position);
    shader->set_vec3("spotLight.ambient", glm::vec3(0.0f));
    shader->set_vec3("spotLight.diffuse", glm::vec3(1.0f, 0.95f, 0.85f));
    shader->set_vec3("spotLight.specular", glm::vec3(1.0f));
    shader->set_vec3("spotLight.clq", glm::vec3(1.0f, 0.09f, 0.032f));

    shader->set_vec3("spotLight.direction", glm::normalize(agent_position - ship_position));
    shader->set_float("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shader->set_float("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

    shader->set_float("material.shi", 32.0f);

    shader->set_vec3("dirLight.dir", glm::vec3(100.0f, -100.0f, 0.0f));
    shader->set_vec3("dirLight.ambient", glm::vec3(0.0f, 0.0f, 0.5f));
    // shader->set_vec3("dirLight.diffuse", glm::vec3(4.0f, 4.0f, 5.0f)); // 0.8f
    shader->set_vec3("dirLight.diffuse", glm::vec3(0.9f, 0.85f, 0.7f));
    shader->set_vec3("dirLight.specular", glm::vec3(0.3f));

    shader->set_float("blin", BlinPhong);

    moon->draw(shader);
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    engine::resources::Shader *shader = resources->shader("skybox");
    auto skybox_cube = resources->skybox("blue_space");

    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
}

void app::MainController::draw() {

    if (pressedB) {
        this->bloomControler->prepare_hdr();
    }

    draw_ship();
    draw_island();

    if (agent_alive) {
        draw_agent();
    }

    draw_moon();

    draw_skybox();

    if (pressedB) {
        this->bloomControler->finalize();
        // this->bloomControler->set_enable(!this->bloomControler->is_bloom_enabled());
    }
}

void app::MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

std::string_view app::MainController::name() const {
    return "app::MainController";
}

}// namespace app