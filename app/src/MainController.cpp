//
// Created by natasa on 2/22/26.
//

#include "../include/MainController.h"

#include "../../engine/libs/glfw/include/GLFW/glfw3.h"
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
static glm::vec3 ship_position = glm::vec3(2.8f, 3.2f, -9.5f);
// static glm::vec3 ship_position = glm::vec3(-3.0f, 3.0f, -10.0f);
static glm::vec3 island_position = glm::vec3(0.0f, 0.0f, -10.0f);
static glm::vec3 agent_position = glm::vec3(0.0f, 0.0f, -8.7f); // y = sint
static glm::vec3 agent_position_origin = glm::vec3(0.0f, 0.0f, -8.7f);
// 3.0, 4.0, -10.0
static glm::vec3 moon_position = glm::vec3(-3.0f, 3.0f, -10.0f);
// static glm::vec3 moon_position = glm::vec3(3.0f, 4.0f, -10.0f);

static bool agent_alive = true;
static bool bloomEnabled = false;
static bool pressedB = false;
static bool BlinPhong = false;
static bool pressedF = false;

static bool pressedE = false;
static bool eventActive = false;
static float eventStartTime = 0.0f;
static bool shipLightBoost = false;
static bool agentGlowBoost = false;

// static glm::vec3 spotLightDiffuse_origin = glm::vec3(1.0f, 0.95f, 0.85f);

void app::MainController::initialize() {
    spdlog::info("MainController initialized....");

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    // platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());

    this->bloomControler = get<engine::graphics::BloomController>();
    this->bloomControler->init();
    this->bloomControler->set_bloom(false);

    engine::graphics::OpenGL::enable_depth_testing();
}

bool app::MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }

    // if (platform->key(engine::platform::KeyId::KEY_B).is_down()) {
    //     if (!pressedB) {
    //         // spotLightDiffuse_origin = glm::vec3(10.0f);
    //         this->bloomControler->toggle_bloom();
    //         pressedB = true;
    //     } else {
    //         // spotLightDiffuse_origin = glm::vec3(1.0f, 0.95f, 0.85f);
    //         pressedB = false;
    //     }
    // }

    // if (platform->key(engine::platform::KeyId::KEY_F).is_down()) {
    //     if (!BlinPhong) {
    //         BlinPhong = true;
    //     } else {
    //         BlinPhong = false;
    //     }
    // }

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

        eventActive = false;
        shipLightBoost = false;
        agentGlowBoost = false;
    }
}

void MainController::update() {
    update_camera();
    update_agent();

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    bool bIsPressed = platform->key(engine::platform::KeyId::KEY_B).is_down();

    if (bIsPressed && !pressedB) {
        bloomEnabled = !bloomEnabled;
        this->bloomControler->set_bloom(bloomEnabled);
    }

    pressedB = bIsPressed;

    bool fIsPressed = platform->key(engine::platform::KeyId::KEY_F).is_down();

    if (fIsPressed && !pressedF) {
        BlinPhong = !BlinPhong;
    }

    pressedF = fIsPressed;

    bool eIsPressed = platform->key(engine::platform::KeyId::KEY_E).is_down();

    if (eIsPressed && !pressedE && agent_alive) {
        eventActive = true;
        eventStartTime = platform->frame_time().current;

        shipLightBoost = false;
        agentGlowBoost = false;
    }

    pressedE = eIsPressed;

    if (eventActive) {
        float esp = platform->frame_time().current - eventStartTime;

        // after 2s
        if (esp > 2.0f) {
            shipLightBoost = true;
        }

        // after 4s
        if (esp > 4.0f) {
            agentGlowBoost = true;
        }

        // after 6s
        if (esp > 6.0f) {
            agent_alive = false;
            eventActive = false;
        }
    }
}

void app::MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::set_lights(engine::resources::Shader* shader)
{
    shader->set_vec3("dirLight.dir", glm::normalize(glm::vec3(-0.4f, -1.0f, -0.2f)));
    // shader->set_vec3("dirLight.dir", glm::vec3(0.1f, 0.02f, 0.5f));
    shader->set_vec3("dirLight.ambient", glm::vec3(0.06f, 0.06f, 0.1f));
    shader->set_vec3("dirLight.diffuse", glm::vec3(0.30f, 0.34f, 0.48f));
    shader->set_vec3("dirLight.specular", glm::vec3(0.25f, 0.25f, 0.35f));

    shader->set_vec3("spotLight.pos", ship_position);
    shader->set_vec3("spotLight.direction", glm::normalize(agent_position - ship_position));
    shader->set_vec3("spotLight.clq", glm::vec3(1.0f, 0.045f, 0.0075f));

    shader->set_float("spotLight.cutOff", glm::cos(glm::radians(12.0f)));
    shader->set_float("spotLight.outerCutOff", glm::cos(glm::radians(17.0f)));

    shader->set_vec3("spotLight.ambient", glm::vec3(0.0f));

    if (shipLightBoost) {
        shader->set_vec3("spotLight.diffuse", glm::vec3(2.0f, 1.85f, 1.25f));
        shader->set_vec3("spotLight.specular", glm::vec3(2.0f, 1.85f, 1.25f));
    } else {
        shader->set_vec3("spotLight.diffuse", glm::vec3(1.0f, 0.95f, 0.75f));
        shader->set_vec3("spotLight.specular", glm::vec3(1.0f, 0.95f, 0.85f));
    }
    shader->set_bool("blin", BlinPhong);
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

    set_lights(shader);

    shader->set_float("material.shi", 96.0f);

    shader->set_vec3("emissive", glm::vec3(0.30f, 0.45f, 0.80f));
    shader->set_float("emissiveStrength", 0.25f);

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

    set_lights(shader);

    shader->set_float("material.shi", 16.0f);

    shader->set_vec3("emissive", glm::vec3(0.0f));
    shader->set_float("emissiveStrength", 0.0f);

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

    set_lights(shader);

    shader->set_float("material.shi", 64.0f);

    if (agentGlowBoost) {
        shader->set_vec3("emissive", glm::vec3(0.0f, 0.75f, 0.95f));
        shader->set_float("emissiveStrength", 1.2f);
    } else {
        shader->set_vec3("emissive", glm::vec3(0.0f, 0.45f, 0.55f));
        shader->set_float("emissiveStrength", 0.5f);
    }
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
    M = glm::translate(M, glm::vec3(-3.0f, 4.7f, 0.0f));
    M = glm::rotate(M, t * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
    M = glm::scale(M, glm::vec3(0.085f));

    shader->set_mat4("M", M);

    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();

    shader->set_float("material.shi", 8.0f);


    set_lights(shader);

    shader->set_vec3("emissive", glm::vec3(0.4f, 0.4f, 0.52f));
    shader->set_float("emissiveStrength", 0.55f);

    moon->draw(shader);
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    engine::resources::Shader *shader = resources->shader("skybox");
    auto skybox_cube = resources->skybox("blue_space");

    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
}

void app::MainController::draw() {

    this->bloomControler->prepare_hdr();

    draw_ship();
    draw_island();

    if (agent_alive) {
        draw_agent();
    }

    draw_moon();

    draw_skybox();

    this->bloomControler->finalize();

}

void app::MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

std::string_view app::MainController::name() const {
    return "app::MainController";
}

}// namespace app