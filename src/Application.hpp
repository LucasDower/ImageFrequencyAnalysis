#pragma once

#include "AppContext.hpp"
#include <GLFW/glfw3.h>

class Application
{
public:
    Application() = default;
    ~Application();

    void Init();

    void MainLoop();

private:
    GLFWwindow* m_Window;
    AppContext m_Context;
};