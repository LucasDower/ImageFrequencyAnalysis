#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "FastDCTLee.hpp"
#include "ImageFrequencyAnalysis.h"
#include "ImageManager.hpp"
#include "MathUtil.hpp"


#include <iostream>
#include <algorithm>

using namespace std;

// image handles
unsigned int g_input_image = 0;
unsigned int g_input_dct = 0;

GLsizei SCR_WIDTH = 800;
GLsizei SCR_HEIGHT = 600;

int image_width = 0;
int image_height = 0;

unsigned char* image_buffer;

float worldx = 0;
float worldy = 0;

double k = 1.0;
double centreX = 0.0f;
double centreY = 0.0f;
double z = 1.0;


void display(GLFWwindow* window)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Input image
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_input_image);

    glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(-0.3f, -0.3f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(0.3f, -0.3f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(0.3f, 0.3f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-0.3f, 0.3f);
        glTexCoord2f(0.0f, 1.0f);
    glEnd();

    //glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_input_dct);

    // Original DCT
    glTranslatef(0.7f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(-0.3f, -0.3f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(0.3f, -0.3f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(0.3f, 0.3f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-0.3f, 0.3f);
        glTexCoord2f(0.0f, 1.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    // Modified DCT
    glTranslatef(0.7f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(-0.3f, -0.3f);
        glVertex2f(0.3f, -0.3f);
        glVertex2f(0.3f, 0.3f);
        glVertex2f(-0.3f, 0.3f);
    glEnd();

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
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    g_input_image = ImageHandler::loadAndBindTexture(&image_buffer, "C:/Users/Lucas/source/repos/ImageFrequencyAnalysis/resources/house.jpg", &image_width, &image_height);
    
    if (!MathUtil::isPowerOf2(image_width * image_height))
    {
        std::cout << "Image dimensions must be a power of two" << std::endl;
        return -1;
    }
    
    // TODO: Replace with interleaved and stride
    vector<double> red, green, blue;
    for (int i = 0; i < image_width * image_height * 3; i += 3)
    {
        red.push_back(image_buffer[i]);
        green.push_back(image_buffer[i+1]);
        blue.push_back(image_buffer[i+2]);
    }

    red = MathUtil::dft2D(red, image_width, image_height);
    green = MathUtil::dft2D(green, image_width, image_height);
    blue = MathUtil::dft2D(blue, image_width, image_height);

    double max_ = std::numeric_limits<double>::min();
    for (int i = 0; i < image_width * image_height; i++)
    {
        red[i] = log(1+abs(red[i]));
        green[i] = log(1 + abs(green[i]));
        blue[i] = log(1 + abs(blue[i]));
        max_ = max(max_, red[i]);
        max_ = max(max_, green[i]);
        max_ = max(max_, blue[i]);
    }
    //printf("max_: %f\n", max_);
    double scale_factor = 255.0 / max_;

    unsigned char* new_data = new unsigned char [(long long)image_width * (long long)image_height * 3LL];
    for (int i = 0; i < image_width * image_height; ++i)
    {
        new_data[3 * i] = (unsigned char) (scale_factor * red[i]);
        new_data[3 * i + 1] = (unsigned char)(scale_factor * green[i]);
        new_data[3 * i + 2] = (unsigned char)(scale_factor * blue[i]);
    }
    
    g_input_dct = ImageHandler::bindTexture(new_data, image_width, image_height);

    updateView();
    while (!glfwWindowShouldClose(window))
    {
        display(window);
    }


    std::free(image_buffer);
    delete[] new_data;
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}


void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);

    GLfloat aspect = (GLfloat)width / (GLfloat)height;
    updateView();
}


void recentre()
{
    if (worldx < 0.3 && worldx > -0.3)
    {
        centreX = 0;
    }
    else if (worldx < 1.0 && worldx > 0.4)
    {
        centreX = 0.7;
    }
    else if (worldx < 1.7 && worldx > 1.1)
    {
        centreX = 1.4;
    }
}

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

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    GLfloat aspect = (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT;
    worldx = (xpos / SCR_WIDTH - 0.5) * aspect * z + centreX;
    worldy = (ypos / SCR_HEIGHT - 0.5) * z + centreY;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
    {
        recentre();
        updateView();
    }
}