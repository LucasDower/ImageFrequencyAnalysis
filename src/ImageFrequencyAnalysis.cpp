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


void draw_input_window(GLFWwindow* window, std::unique_ptr<app_context> const& gui_context)
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
	        }
	        else {
	            printf("Cannot convert greyscale image to RGB. Reload image instead\n");
	        }
	    }

	    if (gui_context->get_input_image_state() == image_state::failed)
	    {
	        const auto error_message = gui_context->get_input_image_error();
	        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), error_message.c_str());
	    }
	
        if (ImGui::Button("Use mask texture"))
        {
            gui_context->get_mask_image()->use_texture();
        }
    ImGui::End();
}


void draw_input_image_window(GLFWwindow* window, std::unique_ptr<app_context> const& gui_context)
{
    if (gui_context->get_input_image_state() != image_state::loaded)
    {
        return;
    }
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), gui_context->get_max_window_size(), square_resize_callback);
    ImGui::Begin("Input Image Preview", nullptr);
	    const auto window_size = ImGui::GetWindowSize();
	    const auto image_size = std::min(window_size.x, window_size.y);
	    const auto& input_image = gui_context->get_input_image();
	    ImGui::Image((void*)(intptr_t)input_image->get_handle(), ImVec2(image_size, image_size));
	    if (ImGui::Button("Perform DCT"))
	    {
	        gui_context->perform_input_dct();
	    }
        if (ImGui::Button("Use texture"))
        {
            gui_context->get_input_image()->use_texture();
        }
    ImGui::End();
}


void draw_dct_window(GLFWwindow* window, std::unique_ptr<app_context> const& gui_context)
{
    if (gui_context->get_input_image_dct_state() != image_state::loaded)
    {
        return;
    }
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), gui_context->get_max_window_size(), square_resize_callback);
    ImGui::Begin("Input Image DCT Preview", nullptr);
	    const auto window_size = ImGui::GetWindowSize();
	    const auto image_size = std::min(window_size.x, window_size.y);
	    const auto input_image_dct = gui_context->get_input_dct_image();
	    ImGui::Image((void*)(intptr_t)input_image_dct->get_handle(), ImVec2(image_size, image_size));
	    if (ImGui::Button("Edit mask"))
	    {
	        gui_context->draw_editing_window = true;
	    }
        if (ImGui::Button("Use texture"))
        {
            gui_context->get_input_dct_image()->use_texture();
        }
    ImGui::End();
}


void setup_windows(GLFWwindow* window, std::unique_ptr<app_context> const& gui_context)
{
	draw_input_window(window, gui_context);
    draw_input_image_window(window, gui_context);
    draw_dct_window(window, gui_context);
}


void draw_editing_window(std::unique_ptr<app_context> const& gui_context)
{
    if (gui_context->draw_editing_window)
    {
        const auto loc = glGetUniformLocation(gui_context->shader_program, "aspect");
        glUniform1f(loc, static_cast<float>(gui_context->get_aspect_ratio()));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
}


void handle_editor_input(GLFWwindow* window, std::unique_ptr<app_context> const& gui_context)
{
	// Ignore mouse input if the user is interacting with an ImGui window
	if (ImGui::GetIO().WantCaptureMouse)
	{
        return;
	}
    if (gui_context->get_mask_state() == image_state::loaded && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
    {
        gui_context->handle_editor();
    }
}


void display(GLFWwindow* window, std::unique_ptr<app_context> const& gui_context)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    setup_windows(window, gui_context);

    ImGui::Render();

    glfwGetFramebufferSize(window, &gui_context->display_width, &gui_context->display_height);
    glViewport(0, 0, gui_context->display_width, gui_context->display_height);
    glClearColor(0.090f, 0.165f, 0.267f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	handle_editor_input(window, gui_context);
    draw_editing_window(gui_context);

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

    glClearColor(0.090f, 0.165f, 0.267f, 1.0f);

    {
        std::string icon_filename("C:/Users/Lucas/source/repos/ImageFrequencyAnalysis/resources/lol.jpg");
        const auto icon = std::make_unique<image_handler>(icon_filename);
        const auto icon_ptr = icon->get_data().get();
        GLFWimage icon_image{ icon->get_width(), icon->get_height(),icon_ptr };
        glfwSetWindowIcon(window, 1, &icon_image);
    }
	
	
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glfwGetCursorPos(window, &gui_context->cursor_x, &gui_context->cursor_y);
        display(window, gui_context);
    	
        glfwSwapBuffers(window);
    }
	
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    gui_context->destroy_buffers();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
