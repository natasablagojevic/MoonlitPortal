//
// Created by natasa on 2/22/26.
//

#ifndef MATF_RG_PROJECT_MAINCONTROLLER_H
#define MATF_RG_PROJECT_MAINCONTROLLER_H
#include "engine/core/Controller.hpp"
#include "engine/graphics/GraphicsController.hpp"
#include <engine/graphics/BloomController.h>

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

    engine::graphics::BloomController *bloomControler;

public:
    std::string_view name() const override;
};

}// namespace app

#endif//MATF_RG_PROJECT_MAINCONTROLLER_H
