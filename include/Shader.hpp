#pragma once

#include <glm/ext.hpp>
#include <string>

class Shader
{
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniform2f(const std::string& name, float v0, float v1);
    void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
    void SetUniform1i(const std::string& name, const int i);
private:
    unsigned int m_RendererID;

    std::string ReadFile(const std::string& filepath);
    unsigned int CompileShader(unsigned int type, const std::string& source);
};
