#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "FastDCTLee.hpp"
#include "ImageFrequencyAnalysis.h"
#include "ImageManager.hpp"

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
    //float currentFrame = glfwGetTime();
        //deltaTime = currentFrame - lastFrame;
        //lastFrame = currentFrame;

        //processInput(window);

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

constexpr bool is_power_of_2(int n)
{
    return (n & (n - 1)) == 0;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    g_input_image = load_and_bind_texture(&image_buffer, "C:/Users/Lucas/source/repos/ImageFrequencyAnalysis/resources/house.jpg", &image_width, &image_height);
    
    if (!is_power_of_2(image_width * image_height))
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

    FastDctLee::transform(red);
    FastDctLee::transform(green);
    FastDctLee::transform(blue);

    double max_ = max(max(red[0], green[0]), blue[0]);

    double in = log10(1 + max_);
    double scale_factor = 255.0 / in;
    std::cout << scale_factor << '\n';

    std::cout << red[0] << " max\n";
    std::cout << (scale_factor * log10(1 + abs(red[0]))) << " test\n";

    unsigned char* new_data = new unsigned char [(long long)image_width * (long long)image_height * 3LL];
    for (int i = 0; i < image_width * image_height; ++i)
    {
        new_data[3*i] = (unsigned char) (scale_factor * log10(1 + abs(red[i])));
        new_data[3 * i + 1] = (unsigned char)(scale_factor * log10(1 + abs(red[i])));
        new_data[3 * i + 2] = (unsigned char)(scale_factor * log10(1 + abs(red[i])));
    }
    
    g_input_dct = bind_texture(new_data, image_width, image_height);

    update_view();
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


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);

    GLfloat aspect = (GLfloat)width / (GLfloat)height;
    update_view();
}


void recentre()
{
    if (worldx < 0.3 && worldx > -0.3)
    {
        centreX = 0;
    }
    if (worldx < 1.0 && worldx > 0.4)
    {
        centreX = 0.7;
    }
    if (worldx < 1.7 && worldx > 1.1)
    {
        centreX = 1.4;
    }
}

void update_view()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat aspect = (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT;
    //recentre();
    glOrtho(centreX - aspect * (z / 2.0), centreX + aspect * (z / 2.0), centreY - (z / 2.0), centreY + (z / 2.0), -1.0, 1.0);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    z -= yoffset * 0.05;
    update_view();
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    GLfloat aspect = (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT;
    worldx = (xpos / SCR_WIDTH - 0.5) * aspect * z + centreX;
    worldy = (ypos / SCR_HEIGHT - 0.5) * z + centreY;
    //std::cout << (xpos/SCR_WIDTH - 0.5) * aspect * z << ", " << (ypos/SCR_HEIGHT - 0.5) * z << '\n';
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    std::cout << button << ' ' << action << ' ' << mods << '\n';
    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
    {
        recentre();
        update_view();
    }
}