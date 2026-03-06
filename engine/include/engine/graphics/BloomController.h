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
    int bloom_passes = 8;
    float exposure = 1.1f;
    bool bloom = true;
    float bloom_intensity = 1.4f;

    void toggle_bloom();
    void set_bloom(bool enabled);
    bool is_bloom_enabled() const;

    void init();
    void render();
    void prepare_hdr();
    void finalize();

    std::string_view name() const override;
    void terminate() override;

private:
    unsigned m_pingpongFBO[2];
    unsigned m_pingpongColorbuffers[2];
    unsigned m_hdrFBO;
    unsigned m_colorBuffers[2];
    unsigned m_width;
    unsigned m_height;
    unsigned m_quadVAO;
    unsigned m_quadVBO;

    void render_quad();
};

}// namespace engine::graphics

#endif//MATF_RG_PROJECT_BLOOMCONTROLLER_H
