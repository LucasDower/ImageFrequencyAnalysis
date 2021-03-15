#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <cstdio>
#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "AppContext.hpp"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "[GLFW]: %d: %s\n", error, description);
}


void display(GLFWwindow* window, std::unique_ptr<app_context>& gui_context)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
	
    ImGui::Begin("Input Image", nullptr);
    auto filename_buffer = gui_context->get_filename_buffer();
    ImGui::InputText("Filename", &filename_buffer[0], filename_buffer.size(), 0, nullptr, nullptr);
    if (ImGui::Button("Load"))
        gui_context->load_input_image();
    ImGui::End();

	
    if (gui_context->input_image_loaded())
    {
        ImGui::Begin("Input Image", nullptr);
        const auto input_image = gui_context->get_input_image();
        ImGui::Image((void*)(intptr_t)input_image->get_handle(), ImVec2(input_image->get_width(), input_image->get_height()));
        ImGui::End();
    }
    

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.7f, 0.141592f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return -1;

    const auto *const glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    auto *window = glfwCreateWindow(1280, 720, "Image Frequency Tool", nullptr, nullptr);
    if (!window)
        return -1;
	
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable v-sync

	// OpenGL loader
    if (!gladLoadGL())
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return -1;
    }

    // Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    std::unique_ptr<app_context> gui_context(new app_context);
	
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        display(window, std::move(gui_context));

        glfwSwapBuffers(window);
    }
	
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
