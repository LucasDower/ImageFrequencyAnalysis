#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include "glad/gl.h"
#include <GLFW/glfw3.h>

#include "AppContext.hpp"

void square_resize_callback(ImGuiSizeCallbackData* data)
{
    const auto current_size = data->DesiredSize;
    const auto new_size = std::max(current_size.x, current_size.y);
    data->DesiredSize = ImVec2(new_size - 20, new_size);
}


void draw_input_window(GLFWwindow* window, app_context& gui_context)
{
    ImGui::Begin("Input Image", nullptr);
	    auto& filename_buffer = gui_context.get_filename_buffer();
	    ImGui::InputText("Filename", &filename_buffer[0], filename_buffer.size(), 0, nullptr, nullptr);

	    if (ImGui::Button("Load"))
	    {
	        gui_context.load_input_image();
	    }

		// TODO: Cleanup
	    if (gui_context.get_input_image_state() == image_state::loaded && ImGui::Checkbox("Treat as greyscale", &gui_context.is_input_greyscale))
	    {
	        if (gui_context.is_input_greyscale)
	        {
	            if (!gui_context.get_input_image()->is_greyscale())
	            {
	                gui_context.get_input_image()->collapse_to_greyscale();
	            }
	        }
	        else {
	            printf("Cannot convert greyscale image to RGB. Reload image instead.\n");
	        }
	    }

	    if (gui_context.get_input_image_state() == image_state::failed)
	    {
	        const auto error_message = gui_context.get_input_image_error();
	        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), error_message.c_str());
	    }

        if (gui_context.get_input_image_state() == image_state::loaded && ImGui::Button("Perform DCT"))
        {
            gui_context.perform_input_dct();
        }

        if (gui_context.get_mask_state() == image_state::loaded)
        {
            if (ImGui::Button("Calculate inverse"))
            {
				gui_context.update_inverse();
			}
			ImGui::SliderFloat("Mask overlay", &gui_context.get_mask_image()->mask_overlay, 0.0f, 1.0f);
        }
    ImGui::End();
}


void draw_input_image_window(GLFWwindow* window, app_context& gui_context)
{
    if (gui_context.get_input_image_state() != image_state::loaded)
    {
        return;
    }
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), gui_context.get_max_window_size(), square_resize_callback);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Input Image Preview", nullptr);
	    const auto window_size = ImGui::GetWindowSize();
	    const auto image_size = std::min(window_size.x, window_size.y - 20);
	    const auto& input_image = gui_context.get_input_image();
        ImTextureID texture_id = (ImTextureID)(intptr_t)input_image->get_handle();
	    ImGui::Image(texture_id, ImVec2(image_size, image_size));
    ImGui::End();
    ImGui::PopStyleVar();
}


void draw_dct_window(GLFWwindow* window, app_context& gui_context)
{
    if (gui_context.get_input_image_dct_state() != image_state::loaded)
    {
        return;
    }
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), gui_context.get_max_window_size(), square_resize_callback);
    ImGui::Begin("Input Image DCT Preview", nullptr);
	    const auto window_size = ImGui::GetWindowSize();
	    const auto image_size = std::min(window_size.x, window_size.y);
	    const auto input_image_dct = gui_context.get_input_dct_image();
        gui_context.get_mask_image()->use_texture();
        ImTextureID texture_id = (ImTextureID)(intptr_t)input_image_dct->get_handle();
        ImGui::Image(texture_id, ImVec2(image_size, image_size));
	    gui_context.draw_editing_window = true;
    ImGui::End();
}


void draw_output_window(GLFWwindow* window, app_context& gui_context)
{
    if (gui_context.get_output_image_state() != image_state::loaded)
    {
        return;
    }
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), gui_context.get_max_window_size(), square_resize_callback);
    ImGui::Begin("Output Image", nullptr);
	    const auto window_size = ImGui::GetWindowSize();
	    const auto image_size = std::min(window_size.x, window_size.y);
	    const auto output_image_dct = gui_context.get_output_image();
        ImTextureID texture_id = (ImTextureID)(intptr_t)output_image_dct->get_handle();
	    ImGui::Image(texture_id, ImVec2(image_size, image_size));
    ImGui::End();
}


void setup_windows(GLFWwindow* window, app_context& gui_context)
{
    draw_input_window(window, gui_context);
    draw_input_image_window(window, gui_context);
    draw_dct_window(window, gui_context);
    draw_output_window(window, gui_context);
}


void draw_editing_window(app_context& gui_context)
{
    if (gui_context.draw_editing_window)
    {
        const auto loc = glGetUniformLocation(gui_context.shader_program, "aspect");
        glUniform1f(loc, static_cast<float>(gui_context.get_aspect_ratio()));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
}


void handle_editor_input(GLFWwindow* window, app_context& gui_context)
{
	// Ignore mouse input if the user is interacting with an ImGui window
	if (ImGui::GetIO().WantCaptureMouse)
	{
        return;
	}
    if (gui_context.get_mask_state() == image_state::loaded && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
    {
        gui_context.handle_editor();
    }
}


void display(GLFWwindow* window, app_context& gui_context)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    setup_windows(window, gui_context);

    ImGui::Render();

    glfwGetFramebufferSize(window, &gui_context.display_width, &gui_context.display_height);
    glViewport(0, 0, gui_context.display_width, gui_context.display_height);
    glClearColor(0.090f, 0.165f, 0.267f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	handle_editor_input(window, gui_context);
    draw_editing_window(gui_context);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}