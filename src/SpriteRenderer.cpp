#include "SpriteRenderer.hpp"

SpriteRenderer::SpriteRenderer(Shader& shader)
    : m_shader(shader)
{
    float vertices[] = {
        // X      Y      Z      U      V
        -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteBuffers(1, &m_EBO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void SpriteRenderer::begin(const glm::mat4& viewProj)
{
    m_viewProj = viewProj;
    m_shader.Bind();
    m_shader.SetUniform1i("u_Texture", 0);
    glBindVertexArray(m_VAO);
}

void SpriteRenderer::draw(
    const Texture2D& texture,
    glm::vec2 pos,
    glm::vec2 size,
    glm::vec2 uvOffset,
    glm::vec2 uvScale)
{
    m_shader.SetUniform2f("u_UVScale", uvScale.x, uvScale.y);
    m_shader.SetUniform2f("u_UVOffset", uvOffset.x, uvOffset.y);

    texture.bind();

    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
    model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));

    auto mvpCoin = m_viewProj * model;
    m_shader.SetUniformMat4f("u_MVP", mvpCoin);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void SpriteRenderer::drawFrame(
    const Texture2D& texture,
    const SpriteFrame& frame,
    glm::vec2 pos,
    glm::vec2 size)
{
    glm::vec2 uvOffset = { frame.x / texture.getWidth(), frame.y / texture.getHeight() };
    glm::vec2 uvScale  = { frame.w / texture.getWidth(), frame.h / texture.getHeight() };

    draw(texture, pos, size, uvOffset, uvScale);
}
