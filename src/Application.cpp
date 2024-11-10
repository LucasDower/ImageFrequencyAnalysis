#include "Application.hpp"
#include "ImageFrequencyAnalysis.h"

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

    m_Context.destroy_buffers();

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
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        const char* GlslVersion = "#version 130";
        ImGui_ImplOpenGL3_Init(GlslVersion);
    }
    
    // Context
    {
        m_Context.Init();
    }

    // Style
    {
        glClearColor(0.090f, 0.165f, 0.267f, 1.0f);
    }
}

void Application::MainLoop()
{
    while (!glfwWindowShouldClose(m_Window))
    {
        glfwPollEvents();

        glfwGetCursorPos(m_Window, &m_Context.cursor_x, &m_Context.cursor_y);

        HandleFrame();

        glfwSwapBuffers(m_Window);
    }
}

void Application::HandleFrame()
{
    display(m_Window, m_Context);
}