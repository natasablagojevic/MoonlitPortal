//
// Created by natasa on 2/27/26.
//

#include "../include/engine/graphics/BloomController.h"

#include "engine/graphics/OpenGL.hpp"
#include "engine/platform/PlatformController.hpp"
#include "engine/resources/ResourcesController.hpp"
#include "spdlog/spdlog.h"

namespace engine::graphics {

// BloomController::~BloomController() {
//     terminate();
// }

void BloomController::toggle_bloom() {
    this->bloom = !this->bloom;
}

void BloomController::set_bloom(bool enabled) {
    this->bloom = enabled;
}

bool BloomController::is_bloom_enabled() const {
    return this->bloom;
}

void BloomController::init() {
    CHECKED_GL_CALL(glGenFramebuffers, 1, &this->m_hdrFBO);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, this->m_hdrFBO);

    CHECKED_GL_CALL(glGenTextures, 2, this->m_colorBuffers);

    this->m_width = get<platform::PlatformController>()->window()->width();
    this->m_height = get<platform::PlatformController>()->window()->height();

    for (unsigned i = 0; i < 2; i++) {
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, this->m_colorBuffers[i]);

        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16F, this->m_width, this->m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->m_colorBuffers[i], 0);
    }


    CHECKED_GL_CALL(glGenRenderbuffers, 1, &this->m_depthRBO);
    CHECKED_GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, this->m_depthRBO);
    CHECKED_GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->m_width, this->m_height);
    CHECKED_GL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->m_depthRBO);

    unsigned attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    CHECKED_GL_CALL(glDrawBuffers, 2, attachments);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);

    CHECKED_GL_CALL(glGenFramebuffers, 2, this->m_pingpongFBO);
    CHECKED_GL_CALL(glGenTextures, 2, this->m_pingpongColorbuffers);

    for (unsigned i = 0; i < 2; i++) {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, this->m_pingpongFBO[i]);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, this->m_pingpongColorbuffers[i]);
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16F, this->m_width, this->m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->m_pingpongColorbuffers[i], 0);

        CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->m_pingpongColorbuffers[i], 0);
    }

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);

    const auto resources = get<resources::ResourcesController>();
    resources::Shader *shader = resources->shader("bloom");
    shader->use();
    shader->set_int("Scene", 0);
    shader->set_int("Blur", 1);
}

void BloomController::render() {
    const auto resources = get<resources::ResourcesController>();
    resources::Shader *blur_shader = resources->shader("blur");
    resources::Shader *bloom_shader = resources->shader("bloom");

    blur_shader->use();
    blur_shader->set_int("image", 0);

    // blur pass
    bool horizontal = true;
    bool first = true;

    for (unsigned i = 0; i < this->bloom_passes; i++) {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, this->m_pingpongFBO[horizontal]);

        blur_shader->set_int("horizontal", horizontal);

        CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, first ? this->m_colorBuffers[1] : this->m_pingpongColorbuffers[!horizontal]);

        CHECKED_GL_CALL(glDisable, GL_DEPTH_TEST);

        render_quad();

        CHECKED_GL_CALL(glEnable, GL_DEPTH_TEST);

        horizontal = !horizontal;

        if (first) {
            first = false;
        }
    }

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    CHECKED_GL_CALL(glViewport, 0, 0, this->m_width, this->m_height);
    CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // final pass

    bloom_shader->use();

    bloom_shader->set_int("Scene", 0);
    bloom_shader->set_int("Blur", 1);

    bloom_shader->set_bool("useBloom", this->bloom);
    bloom_shader->set_float("bloomIntensity", this->bloom_intensity);
    bloom_shader->set_float("exposure", this->exposure);

    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, this->m_colorBuffers[0]);

    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE1);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, this->m_pingpongColorbuffers[!horizontal]);

    CHECKED_GL_CALL(glDisable, GL_DEPTH_TEST);
    render_quad();
    CHECKED_GL_CALL(glEnable, GL_DEPTH_TEST);
}

void BloomController::prepare_hdr() {
    auto platform = get<platform::PlatformController>();

    unsigned width = platform->window()->width();
    unsigned height = platform->window()->height();

    resize(width, height);

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, this->m_hdrFBO);
    CHECKED_GL_CALL(glViewport, 0, 0, this->m_width, this->m_height);
    CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BloomController::finalize() {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    CHECKED_GL_CALL(glViewport, 0, 0, this->m_width, this->m_height);
    render();
}
void BloomController::resize(int Width, int Height) {

    if (Width == 0 || Height == 0) {
        return ;
    }

    if (Width == this->m_width && Height == this->m_height) {
        return ;
    }

    this->m_height = Height;
    this->m_width =  Width;

    // HDR : scene + bright
    for (auto i = 0; i < 2; i++) {
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, this->m_colorBuffers[i]);
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16F, this->m_width, this->m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    }

    // Depth renderbuffer
    CHECKED_GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, this->m_depthRBO);
    CHECKED_GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->m_width, this->m_height);

    // Ping-Pong blur textures
    for (auto i = 0; i < 2; i++) {
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, this->m_pingpongColorbuffers[i]);
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16F, this->m_width, this->m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    }

    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
    CHECKED_GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, 0);
}

std::string_view BloomController::name() const {
    return "engine::graphics::BloomController";
}

void BloomController::terminate() {
    if (this->m_hdrFBO) {
        CHECKED_GL_CALL(glDeleteFramebuffers, 1, &this->m_hdrFBO);
    }

    CHECKED_GL_CALL(glDeleteFramebuffers, 2, this->m_pingpongFBO);
    CHECKED_GL_CALL(glDeleteTextures, 2, this->m_colorBuffers);
    CHECKED_GL_CALL(glDeleteTextures, 2, this->m_pingpongColorbuffers);

    if (this->m_depthRBO) {
        CHECKED_GL_CALL(glDeleteRenderbuffers, 1, &this->m_depthRBO);
    }

    if (this->m_quadVAO) {
        CHECKED_GL_CALL(glDeleteVertexArrays, 1, &this->m_quadVAO);
    }

    if (this->m_quadVBO) {
        CHECKED_GL_CALL(glDeleteBuffers, 1, &this->m_quadVBO);
    }

    this->m_quadVAO = 0;
    this->m_quadVBO = 0;
}

void BloomController::render_quad() {
    if (this->m_quadVAO == 0) {
        float quadVertices[] = {
            // positions           // texCoords
            -1.0f,  1.0f, 0.0f,    0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,    0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,    1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,    1.0f, 0.0f
        };

        CHECKED_GL_CALL(glGenVertexArrays, 1, &this->m_quadVAO);;
        CHECKED_GL_CALL(glGenBuffers, 1, &this->m_quadVBO);

        CHECKED_GL_CALL(glBindVertexArray, this->m_quadVAO);
        CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, this->m_quadVBO);
        CHECKED_GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        CHECKED_GL_CALL(glEnableVertexAttribArray, 0);
        CHECKED_GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);

        CHECKED_GL_CALL(glEnableVertexAttribArray, 1);
        CHECKED_GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    }

    CHECKED_GL_CALL(glBindVertexArray, this->m_quadVAO);
    CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
    CHECKED_GL_CALL(glBindVertexArray, 0);
}

}// namespace engine::graphics