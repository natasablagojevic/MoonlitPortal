//
// Created by natasa on 2/27/26.
//

#ifndef MATF_RG_PROJECT_BLOOMCONTROLLER_H
#define MATF_RG_PROJECT_BLOOMCONTROLLER_H
#include "engine/core/Controller.hpp"


#include <glad/glad.h>

namespace engine::graphics {

class BloomController final : public core::Controller{
public:
    // BloomController() = default;
    // ~BloomController();

    int bloom_passes = 8;
    float exposure = 1.1f;
    bool bloom = false;
    float bloom_intensity = 1.4f;

    void toggle_bloom();
    void set_bloom(bool enabled);
    bool is_bloom_enabled() const;

    void init();
    void render();
    void prepare_hdr();
    void finalize();

    void resize(int Width, int Height);

    std::string_view name() const override;
    void terminate() override;void camera();

private:
    unsigned m_pingpongFBO[2] = {};
    unsigned m_pingpongColorbuffers[2] = {};
    unsigned m_hdrFBO = 0;
    unsigned m_colorBuffers[2] = {};
    unsigned m_width = 0;
    unsigned m_height = 0;
    unsigned m_quadVAO = 0;
    unsigned m_quadVBO = 0;
    unsigned m_depthRBO = 0;

    void render_quad();
};

}// namespace engine::graphics

#endif//MATF_RG_PROJECT_BLOOMCONTROLLER_H
