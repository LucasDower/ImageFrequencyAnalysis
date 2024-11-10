#pragma once

void square_resize_callback(ImGuiSizeCallbackData* data);

void draw_input_window(GLFWwindow* window, app_context& gui_context);

void draw_input_image_window(GLFWwindow* window, app_context& gui_context);

void draw_dct_window(GLFWwindow* window, app_context& gui_context);

void draw_output_window(GLFWwindow* window, app_context& gui_context);

void setup_windows(GLFWwindow* window, app_context& gui_context);

void draw_editing_window(app_context& gui_context);

void handle_editor_input(GLFWwindow* window, app_context& gui_context);

void display(GLFWwindow* window, app_context& gui_context);