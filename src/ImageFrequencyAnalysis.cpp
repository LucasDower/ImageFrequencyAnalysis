#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "FastDCTLee.hpp"
#include "ImageFrequencyAnalysis.h"
#include "ImageManager.hpp"
#include "MathUtil.hpp"

#include <iostream>
#include <algorithm>

using namespace std;

enum class Panel { input_spatial, input_frequency, mask, output_frequency, output_spatial};

// image handles
unsigned int g_input_image = 0;
unsigned int g_input_dct = 0;
unsigned int g_mask = 0;
unsigned int g_output_dct = 0;
unsigned int g_output_image = 0;

GLsizei SCR_WIDTH = 800;
GLsizei SCR_HEIGHT = 600;

int image_width = 0;
int image_height = 0;

int brush_size = 5;

Panel focussedPanel = Panel::input_spatial;

//unsigned char* image_buffer;
unsigned char* mask;
double* input_dct;
unsigned char* input_dct_display;
unsigned char* output_dct;
unsigned char* output_dct_display;
unsigned char* output_image;


bool isBrushDown = false;

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

    glEnable(GL_TEXTURE_2D);

    // Input image
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

    // Input DCT
    glBindTexture(GL_TEXTURE_2D, g_input_dct);
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

    // Mask
    glBindTexture(GL_TEXTURE_2D, g_mask);
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

    // Output DCT
    glBindTexture(GL_TEXTURE_2D, g_output_dct);
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

    // Output Image
    glBindTexture(GL_TEXTURE_2D, g_output_image);
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

    glfwSwapBuffers(window);
    glfwPollEvents();
}

inline void setPixelColour(int x, int y)
{
    int index = 3 * (y * image_width + x);

    if (index < 0 || index >= image_width * image_height * 3)
    {
        return;
    }

    mask[index] = 0;
    mask[index + 1] = 0;
    mask[index + 2] = 0;

    output_dct_display[index] = 0;
    output_dct_display[index + 1] = 0;
    output_dct_display[index + 2] = 0;
}

void updateMaskTexture()
{
    glBindTexture(GL_TEXTURE_2D, g_mask);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, mask);
}

void updateOutputDCTTexture()
{
    glBindTexture(GL_TEXTURE_2D, g_output_dct);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, output_dct_display);
}

void updateMask()
{
    if (!isBrushDown)
    {
        return;
    }

    float x01 = (worldx - centreX + 0.3) / 0.6;
    float y01 = (worldy - centreY + 0.3) / 0.6;
    int px = image_width * x01;
    int py = image_height * y01;

    for (int i = -brush_size; i < brush_size; ++i)
    {
        for (int j = -brush_size; j < brush_size; ++j)
        {
            setPixelColour(px + i, py + j);
        }
    }

    updateMaskTexture();
    updateOutputDCTTexture();    
}

void updateOutputImage()
{
    vector<double> red, green, blue;
    for (int i = 0; i < image_width * image_height * 3; i += 3)
    {
        red.push_back(mask[i] ? input_dct[i] : 0);
        green.push_back(mask[i+1] ? input_dct[i+1] : 0);
        blue.push_back(mask[i+2] ? input_dct[i+2] : 0);
    }

    std::cout << "first few reds\n";
    for (int i = 0; i < 8; i++)
    {
        std::cout << red[i] << std::endl;
    }

    red = MathUtil::dft2D(red, image_width, image_height, -1);
    green = MathUtil::dft2D(green, image_width, image_height, -1);
    blue = MathUtil::dft2D(blue, image_width, image_height, -1);

    output_image = new unsigned char[(long long)image_width * (long long)image_height * 3LL];
    std::cout << red[0] << ' ' << green[0] << ' ' << blue[0] << std::endl;
    for (int i = 0; i < image_width * image_height; ++i)
    {
        output_image[3 * i] = (unsigned char) red[i];
        output_image[3 * i + 1] = (unsigned char) green[i];
        output_image[3 * i + 2] = (unsigned char) blue[i];
    }

    glBindTexture(GL_TEXTURE_2D, g_output_image);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, output_image);
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
    glfwSetKeyCallback(window, keyCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    unsigned char* image_buffer;
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

    red = MathUtil::dft2D(red, image_width, image_height, 1);
    green = MathUtil::dft2D(green, image_width, image_height, 1);
    blue = MathUtil::dft2D(blue, image_width, image_height, 1);

    double max_ = std::numeric_limits<double>::min();
    input_dct = new double[(long long)image_width * (long long)image_height * 3LL];
    for (int i = 0; i < image_width * image_height; i++)
    {
        input_dct[3*i] = red[i];
        input_dct[3 * i + 1] = green[i];
        input_dct[3 * i + 2] = blue[i];

        red[i] = log(1+abs(red[i]));
        green[i] = log(1 + abs(green[i]));
        blue[i] = log(1 + abs(blue[i]));
        max_ = max(max_, red[i]);
        max_ = max(max_, green[i]);
        max_ = max(max_, blue[i]);
    }
    double scale_factor = 255.0 / max_;

    input_dct_display = new unsigned char [(long long)image_width * (long long)image_height * 3LL];
    for (int i = 0; i < image_width * image_height; ++i)
    {
        input_dct_display[3 * i] = (unsigned char) (scale_factor * red[i]);
        input_dct_display[3 * i + 1] = (unsigned char)(scale_factor * green[i]);
        input_dct_display[3 * i + 2] = (unsigned char)(scale_factor * blue[i]);
    }
    
    g_input_dct = ImageHandler::bindTexture(input_dct_display, image_width, image_height);

    mask = new unsigned char[(long long)image_width * (long long)image_height * 3LL];
    output_dct_display = new unsigned char[(long long)image_width * (long long)image_height * 3LL];
    output_image = new unsigned char[(long long)image_width * (long long)image_height * 3LL];
    for (int i = 0; i < image_width * image_height * 3; ++i)
    {
        mask[i] = 255;
        output_dct_display[i] = input_dct_display[i];
        output_image[i] = image_buffer[i];
    }
    std::free(image_buffer);

    g_mask = ImageHandler::bindTexture(mask, image_width, image_height);
    g_output_dct = ImageHandler::bindTexture(output_dct_display, image_width, image_height);
    g_output_image = ImageHandler::bindTexture(output_image, image_width, image_height);

    updateView();
    while (!glfwWindowShouldClose(window))
    {
        display(window);
    }

    
    delete[] input_dct_display;
    delete[] mask;
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
        focussedPanel = Panel::input_spatial;
        centreX = 0;
    }
    else if (worldx < 1.0 && worldx > 0.4)
    {
        focussedPanel = Panel::input_frequency;
        centreX = 0.7;
    }
    else if (worldx < 1.7 && worldx > 1.1)
    {
        focussedPanel = Panel::mask;
        centreX = 1.4;
    }
    else if (worldx < 2.4 && worldx > 1.8)
    {
        focussedPanel = Panel::output_frequency;
        centreX = 2.1;
    }
    else if (worldx < 3.1 && worldx > 2.5)
    {
        focussedPanel = Panel::output_spatial;
        centreX = 2.8;
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
    updateMask();
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
    {
        recentre();
        updateView();
        isBrushDown = (focussedPanel == Panel::mask);
    }
    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE)
    {
        recentre();
        updateView();
        isBrushDown = false;
        updateOutputImage();
    }

}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        for (int i = 0; i < image_width * image_height * 3; ++i)
        {
            mask[i] = 255;
            output_dct_display[i] = input_dct_display[i];
        }
        updateMaskTexture();
        updateOutputDCTTexture();
        updateOutputImage();
    }
}