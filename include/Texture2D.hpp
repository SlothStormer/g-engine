#pragma once

#include <glad/gl.h>
#include <stb_image.h>

#include <string>

class Texture2D
{
private:
    unsigned int m_id;
    int m_width;
    int m_height;

    unsigned int LoadTexture(const char* filepath, GLint wrapS = GL_CLAMP_TO_EDGE);
public:
    Texture2D(const std::string& path, GLint wrapS = GL_CLAMP_TO_EDGE);
    ~Texture2D();

    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    void bind() const;

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
};
