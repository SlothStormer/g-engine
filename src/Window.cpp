#include "Window.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

Window::Window()
{
    if(!glfwInit())
    {
        std::cerr << "Error al iniciar GLFW!" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(800, 600, "OpenGL Test", NULL, NULL);
    if(!m_window)
    {
        std::cerr << "Error al crear la ventana\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);

    int version = gladLoadGL(glfwGetProcAddress);
    if (!version)
    {
        std::cerr << "Error al iniciar GLAD" << std::endl;
    }
    std::cout << "OpenGL cargado con exito. Version: "
                  << GLAD_VERSION_MAJOR(version) << "."
                  << GLAD_VERSION_MINOR(version) << "\n";

    glViewport(0, 0, 800, 600);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(m_window);
}

double Window::getTime()
{
    return glfwGetTime();
}

void Window::pollEvents()
{
    glfwPollEvents();
}

void Window::setShouldClose(bool v)
{
    glfwSetWindowShouldClose(m_window, v);
}

bool Window::keyIsPressed(int key)
{
    return (glfwGetKey(m_window, key) == GLFW_PRESS);
}

void Window::swapBuffers()
{
    glfwSwapBuffers(m_window);
}
