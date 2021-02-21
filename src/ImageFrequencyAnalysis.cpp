#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "FastDCTLee.hpp"
#include "ImageFrequencyAnalysis.h"
#include "ImageManager.hpp"
#include "MathUtil.hpp"
#include "PanelManager.hpp"

#include <iostream>
#include <algorithm>

GLsizei SCR_WIDTH = 800;
GLsizei SCR_HEIGHT = 600;

float worldx = 0;
float worldy = 0;

PanelManager panelManager = PanelManager(800, 600, 0.1f);

void display(GLFWwindow* window)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    panelManager.DrawPanel();

    glfwSwapBuffers(window);
    glfwPollEvents();
}



int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Image Frequency Analysis Tool", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    //glfwSetScrollCallback(window, scrollCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //panelManager = PanelManager();

    //updateView();
    while (!glfwWindowShouldClose(window))
    {
        display(window);
    }

    glfwTerminate();
    return 0;
}


void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);

    GLfloat aspect = (GLfloat)width / (GLfloat)height;
    //updateView();
}

/*
void updateView()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat aspect = (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT;
    glOrtho(centreX - aspect * (z / 2.0), centreX + aspect * (z / 2.0), centreY - (z / 2.0), centreY + (z / 2.0), -1.0, 1.0);
}


void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    z -= yoffset * 0.05;
    z = clamp(z, 0.1, 2.0);
    updateView();
}
*/