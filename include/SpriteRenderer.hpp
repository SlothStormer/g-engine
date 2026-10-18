#pragma once

#include "Shader.hpp"
#include "Texture2D.hpp"
#include <glad/gl.h>
#include <glm/ext/vector_float2.hpp>

struct SpriteFrame
{
    float x, y, w, h;
};

class SpriteRenderer
{
private:
    unsigned int m_VAO, m_VBO, m_EBO;

    Shader& m_shader;
    glm::mat4 m_viewProj;
public:
    SpriteRenderer(Shader& shader);
    ~SpriteRenderer();

    SpriteRenderer(const SpriteRenderer&) = delete;
    SpriteRenderer& operator=(const SpriteRenderer&) = delete;

    void begin(const glm::mat4& viewProj);

    void draw(const Texture2D& texture,
              glm::vec2 pos,
              glm::vec2 size,
              glm::vec2 uvOffset = {0.0f, 0.0f},
              glm::vec2 uvScale = {1.0f, 1.0f});

    void drawFrame(const Texture2D& texture,
                   const SpriteFrame& frame,
                   glm::vec2 pos,
                   glm::vec2 size);
};
