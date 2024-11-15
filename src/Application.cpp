#include "Application.hpp"

#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include "glad/gl.h"
#include <GLFW/glfw3.h>

Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
    }
    glfwTerminate();
}

void Application::Init()
{
    // Init GLFW
    {
        glfwSetErrorCallback([](int ErrorCode, const char* Description)
            {
                std::cerr << "[GLAD]: " << ErrorCode << " - " << Description << "\n";
            });

        if (!glfwInit())
        {
            std::cerr << "Failed to initialise GLFW\n";
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        m_Window = glfwCreateWindow(1280, 720, "Image Frequency Tool", nullptr, nullptr);
        if (!m_Window)
        {
            std::cerr << "Failed to create window\n";
            return;
        }

        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(1);
    }

    // Init GLAD
    {
        if (!gladLoaderLoadGL())
        {
            std::cerr << "Failed to initialise GLAD\n";
            return;
        }
    }

    // Imgui
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        ImGui_ImplOpenGL3_Init("#version 130");

        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }

    // Context
    {
        m_Context.Init();
    }
}

void Application::MainLoop()
{
    while (!glfwWindowShouldClose(m_Window))
    {
        glfwPollEvents();

        m_Context.Update();
        m_Context.Render(*m_Window);

        glfwSwapBuffers(m_Window);
    }
}