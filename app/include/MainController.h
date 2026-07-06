//
// Created by natasa on 2/22/26.
//

#ifndef MATF_RG_PROJECT_MAINCONTROLLER_H
#define MATF_RG_PROJECT_MAINCONTROLLER_H
#include "engine/core/Controller.hpp"
#include "engine/graphics/GraphicsController.hpp"
#include <engine/graphics/BloomController.h>
#include <engine/platform/PlatformController.hpp>

namespace app {

struct Brightness {
    glm::vec3 emissive;
    float emissiveStrength;
};

class MainController : public engine::core::Controller {
private:
    void initialize() override;
    bool loop() override;
    void update() override;
    void begin_draw() override;
    void draw() override;
    void end_draw() override;

    void draw_moon();
    void draw_island();
    void draw_ship();
    void draw_skybox();
    void draw_agent();

    void update_camera();
    void update_agent();

    void set_lights(engine::resources::Shader* shader);

    void handle_key();
    void handleEvent(engine::platform::PlatformController* platform);

    engine::graphics::BloomController *bloomController;

    glm::vec3 ship_position = glm::vec3(2.8f, 3.2f, -9.5f);
    glm::vec3 island_position = glm::vec3(0.0f, 0.0f, -10.0f);
    glm::vec3 agent_position = glm::vec3(0.0f, 0.0f, -8.7f); // y = sint
    glm::vec3 agent_position_origin = glm::vec3(0.0f, 0.0f, -8.7f);
    glm::vec3 moon_position = glm::vec3(-3.0f, 3.0f, -10.0f);

    bool agent_alive = true;
    bool bloomEnabled = false;
    bool BlinPhong = false;
    bool eventActive = false;

    float eventStartTime = 0.0f;
    bool shipLightBoost = false;
    bool agentGlowBoost = false;
    glm::vec3 eventAgentStartPosition;
    glm::vec3 portalTargetPosition = glm::vec3(0.0f, 0.0f,  -11.0f);

public:
    std::string_view name() const override;
    float spotLightStrength = 1.0f;
};

}// namespace app

#endif//MATF_RG_PROJECT_MAINCONTROLLER_H
