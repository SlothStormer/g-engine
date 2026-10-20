    #include "Camara2D.hpp"
Camara2D::Camara2D(float width, float height)
    : m_width(width), m_height(height), m_pos(0.0f, 0.0f)
{
    m_proj = glm::ortho(0.0f, m_width, m_height, 0.0f, -1.0f, 1.0f);
    m_view = glm::mat4(1.0f);
    m_viewProj = m_proj * m_view;
}

void Camara2D::setPosition(glm::vec2 pos)
{
    m_pos = pos;
    m_view = glm::translate(glm::mat4(1.0f), glm::vec3(-m_pos.x, -m_pos.y, 0.0f));
    m_viewProj = m_proj * m_view;
}

glm::vec2 Camara2D::getPosition()
{
    return m_pos;
}

const glm::mat4& Camara2D::getProjection() const
{
    return m_proj;
}

const glm::mat4& Camara2D::getViewProjection() const
{
    return m_viewProj;
}

void Camara2D::setCenter(glm::vec2 center)
{
    setPosition({ center.x - m_width / 2, center.y - m_height / 2,});
}
