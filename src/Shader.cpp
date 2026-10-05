#include "Shader.hpp"
#include <glad/gl.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <glm/gtc/type_ptr.hpp>

// Publicas

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vertexSource = ReadFile(vertexPath);
    std::string fragmentSource = ReadFile(fragmentPath);

    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    m_RendererID = glCreateProgram();
    glAttachShader(m_RendererID, vs);
    glAttachShader(m_RendererID, fs);
    glLinkProgram(m_RendererID);

    int exito;
    glGetProgramiv(m_RendererID, GL_LINK_STATUS, &exito);
    if(!exito)
    {
        char log[512];
        glGetProgramInfoLog(m_RendererID, 512, nullptr, log);
        std::cerr << "Error al linkear Shader:\n" << log << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
    {
        std::cerr << "Aviso: El uniform '" << name << "' no existe o no se esta usando" << std::endl;
    }
    glUniform4f(location, v0, v1, v2, v3);
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
    {
        std::cerr << "Aviso: El uniform '" << name << "' no existe o no se esta usando" << std::endl;
    }
    glUniform2f(location, v0, v1);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
    {
        std::cerr << "Aviso: El uniform '" << name << "' no existe o no se esta usando" << std::endl;
    }
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniform1i(const std::string& name, const int i)
{
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
    {
        std::cerr << "Aviso: El uniform '" << name << "' no existe o no se esta usando" << std::endl;
    }
    glUniform1i(location, i);
}

// Privadas

std::string Shader::ReadFile(const std::string& filepath)
{
    std::ifstream file(filepath);
    if(!file.is_open())
    {
        std::cerr << "Error: No se pudo abrir el archivo: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int exito;
    glGetShaderiv(id, GL_COMPILE_STATUS, &exito);
    if(!exito)
    {
        char log[512];
        glGetShaderInfoLog(id, 512, nullptr, log);
        std::string tipoStr = (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";
        std::cerr << "Error compilando " << tipoStr << " Shader:\n" << log << std::endl;
    }

    return id;
}
