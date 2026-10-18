#include "Texture2D.hpp"

#include <string>
#include <iostream>

Texture2D::Texture2D(const std::string& path, GLint wrapS)
    : m_id(0), m_width(0), m_height(0)
{
    int nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &nrChannels, 4);

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Error al cargar la textura!" << std::endl;
    }

    m_id = textureID;
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &m_id);
}

void Texture2D::bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_id);
}

unsigned int Texture2D::LoadTexture(const char* filepath, GLint wrapS)
{
    int nrChannels;
    unsigned char* data = stbi_load(filepath, &m_width, &m_height, &nrChannels, 4);

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Error al cargar la textura!" << std::endl;
    }

    return textureID;
}
