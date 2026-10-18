#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

class Window
{
private:
    GLFWwindow* m_window;
public:
    Window();
    ~Window();

    bool   shouldClose();
    double getTime();
    void   pollEvents();
    void   setShouldClose(bool v = true);
    bool   keyIsPressed(int key);
    void   swapBuffers();
};
