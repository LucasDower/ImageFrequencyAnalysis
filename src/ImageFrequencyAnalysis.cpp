#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ImageFrequencyAnalysis.h"
#include "FastDCTLee.hpp"
#include "MathUtil.hpp"
#include "TexturePanel.hpp"

#include <iostream>
#include <algorithm>

GLsizei SCR_WIDTH = 800;
GLsizei SCR_HEIGHT = 600;

float worldx = 0;
float worldy = 0;

TexturePanel editingPanel = TexturePanel(-0.25f, 0.2f, 0.2f);
TexturePanel previewPanel = TexturePanel(0.25f, 0.2f, 0.2f);

TexturePanel inputPanel = TexturePanel(-0.25f, -0.225f, 0.1f);
TexturePanel outputPanel = TexturePanel(0.25f, -0.225f, 0.1f);

TexturePanel redDCTPanel = TexturePanel(-0.075f, -0.1f, 0.05f);
TexturePanel redImagePanel = TexturePanel(0.075f, -0.1f, 0.05f);

TexturePanel greenDCTPanel = TexturePanel(-0.075f, -0.225f, 0.05f);
TexturePanel greenImagePanel = TexturePanel(0.075f, -0.225f, 0.05f);

TexturePanel blueDCTPanel = TexturePanel(-0.075f, -0.35f, 0.05f);
TexturePanel blueImagePanel = TexturePanel(0.075f, -0.35f, 0.05f);


void display(GLFWwindow* window)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    editingPanel.DrawBoundary();
    previewPanel.DrawBoundary();

    //inputPanel.DrawBoundary();
    inputPanel.DrawPanel();
    outputPanel.DrawBoundary();

    redDCTPanel.DrawBoundary();
    redImagePanel.DrawBoundary();

    greenDCTPanel.DrawBoundary();
    greenImagePanel.DrawBoundary();

    blueDCTPanel.DrawBoundary();
    blueImagePanel.DrawBoundary();

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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    inputPanel.SetImage("C:/Users/Lucas/source/repos/ImageFrequencyAnalysis/resources/house.jpg");

    updateView();
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
    updateView();
}


void updateView()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat aspect = (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT;
    glOrtho(0.0 - aspect * (1.0 / 2.0), 0.0 + aspect * (1.0 / 2.0), 0.0 - (1.0 / 2.0), 0.0 + (1.0 / 2.0), -1.0, 1.0);
}

/*
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    z -= yoffset * 0.05;
    z = clamp(z, 0.1, 2.0);
    updateView();
}
*/