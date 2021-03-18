#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "AppContext.hpp"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "[GLFW]: %d: %s\n", error, description);
}


void square_resize_callback(ImGuiSizeCallbackData* data)
{
    const auto current_size = data->DesiredSize;
    const auto new_size = std::max(current_size.x, current_size.y);
    data->DesiredSize = ImVec2(new_size, new_size);
}


void setup_windows(GLFWwindow* window, std::unique_ptr<app_context> const& gui_context)
{
    ImGui::Begin("Input Image", nullptr);
    auto& filename_buffer = gui_context->get_filename_buffer();
    ImGui::InputText("Filename", &filename_buffer[0], filename_buffer.size(), 0, nullptr, nullptr);

    if (ImGui::Button("Load"))
    {
        gui_context->load_input_image();
    }

    if (gui_context->get_input_image_state() == image_state::loaded && ImGui::Checkbox("Treat as greyscale", &gui_context->is_input_greyscale))
    {
        if (gui_context->is_input_greyscale)
        {
            if (!gui_context->get_input_image()->is_greyscale())
            {
                gui_context->get_input_image()->collapse_to_greyscale();
            }
        } else {
            printf("Cannot convert greyscale image to RGB. Reload image instead\n");
        }
    }
	
    if (gui_context->get_input_image_state() == image_state::failed)
    {
        const auto error_message = gui_context->get_input_image_error();
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), error_message.c_str());
    }
    ImGui::End();

	// Show input image preview
    if (gui_context->get_input_image_state() != image_state::loaded)
    {
        return;
    }
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(500, 500), square_resize_callback);
    ImGui::Begin("Input Image Preview", nullptr);
    auto window_size = ImGui::GetWindowSize();
    auto image_size = std::min(window_size.x, window_size.y);
    const auto& input_image = gui_context->get_input_image();
    ImGui::Image((void*)(intptr_t)input_image->get_handle(), ImVec2(image_size, image_size));
    if (ImGui::Button("Perform DCT"))
    {
        gui_context->perform_input_dct();
    }
    ImGui::End();

	// Show input image DCT preview
    if (gui_context->get_input_image_dct_state() != image_state::loaded)
    {
        return;
    }
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(500, 500), square_resize_callback);
    ImGui::Begin("Input Image DCT Preview", nullptr);
    window_size = ImGui::GetWindowSize();
    image_size = std::min(window_size.x, window_size.y);
    const auto input_image_dct = gui_context->get_input_dct_image();
    ImGui::Image((void*)(intptr_t)input_image_dct->get_handle(), ImVec2(image_size, image_size));
    ImGui::End();
}


void display(GLFWwindow* window, std::unique_ptr<app_context> const &gui_context)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
	
    setup_windows(window, gui_context);

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.090f, 0.165f, 0.267f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


int main(int, char**)
{
	// GLFW
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

	// GLAD
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

    const std::unique_ptr<app_context> gui_context(new app_context());
	
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        display(window, gui_context);

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
