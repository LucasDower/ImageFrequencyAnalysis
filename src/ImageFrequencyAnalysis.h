#pragma once

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void cursor_position_callback(GLFWwindow* window, double xpos, double pos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void update_view();