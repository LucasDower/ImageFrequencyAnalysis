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
    void HandleFrame();

    GLFWwindow* m_Window;
    app_context m_Context;
};