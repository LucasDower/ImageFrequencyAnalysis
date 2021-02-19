#pragma once

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void cursorPositionCallback(GLFWwindow* window, double xpos, double pos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void updateView();