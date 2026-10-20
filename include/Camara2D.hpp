#pragma once

#include <glm/ext.hpp>

class Camara2D
{
    float m_width, m_height;
    glm::vec2 m_pos;
    glm::mat4 m_proj;
    glm::mat4 m_view;
    glm::mat4 m_viewProj;
public:
    Camara2D(float width, float height);
    ~Camara2D() = default;

    void setPosition(glm::vec2 pos);
    glm::vec2 getPosition();
    const glm::mat4& getProjection() const;
    const glm::mat4& getViewProjection() const;
    void setCenter(glm::vec2 center);
};
