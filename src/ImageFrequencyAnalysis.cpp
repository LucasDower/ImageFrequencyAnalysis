#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <shader.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <iostream>
#include <algorithm>
#include <string>
#include <array>
#include <vector>
#include <complex>
#include <cmath>
#include <stdexcept>

using namespace std;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
float fov = 45;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


short FFT(short int dir, long m, double* x, double* y)
{
    long n, i, i1, j, k, i2, l, l1, l2;
    double c1, c2, tx, ty, t1, t2, u1, u2, z;

    /* Calculate the number of points */
    n = 1;
    for (i = 0; i < m; i++)
        n *= 2;

    /* Do the bit reversal */
    i2 = n >> 1;
    j = 0;
    for (i = 0; i < n - 1; i++) {
        if (i < j) {
            tx = x[i];
            ty = y[i];
            x[i] = x[j];
            y[i] = y[j];
            x[j] = tx;
            y[j] = ty;
        }
        k = i2;
        while (k <= j) {
            j -= k;
            k >>= 1;
        }
        j += k;
    }

    /* Compute the FFT */
    c1 = -1.0;
    c2 = 0.0;
    l2 = 1;
    for (l = 0; l < m; l++) {
        l1 = l2;
        l2 <<= 1;
        u1 = 1.0;
        u2 = 0.0;
        for (j = 0; j < l1; j++) {
            for (i = j; i < n; i += l2) {
                i1 = i + l1;
                t1 = u1 * x[i1] - u2 * y[i1];
                t2 = u1 * y[i1] + u2 * x[i1];
                x[i1] = x[i] - t1;
                y[i1] = y[i] - t2;
                x[i] += t1;
                y[i] += t2;
            }
            z = u1 * c1 - u2 * c2;
            u2 = u1 * c2 + u2 * c1;
            u1 = z;
        }
        c2 = sqrt((1.0 - c1) / 2.0);
        if (dir == 1)
            c2 = -c2;
        c1 = sqrt((1.0 + c1) / 2.0);
    }

    /* Scaling for forward transform */
    if (dir == 1) {
        for (i = 0; i < n; i++) {
            x[i] /= n;
            y[i] /= n;
        }
    }

    return true;
}

int Powerof2(int n, int* m, int* twopm)
{
    if (n <= 1) {
        *m = 0;
        *twopm = 1;
        return false;
    }

    *m = 1;
    *twopm = 2;
    do {
        (*m)++;
        (*twopm) *= 2;
    } while (2 * (*twopm) <= n);

    if (*twopm != n)
        return false;
    else
        return true;
}

int FFT2D(complex<double>** arr, int nx, int ny, int dir)
{
    int i, j;
    int m, twopm;
    double* real, * imag;

    /* Transform the rows */
    real = (double*)malloc(nx * sizeof(double));
    imag = (double*)malloc(nx * sizeof(double));
    if (real == NULL || imag == NULL)
        return false;
    if (!Powerof2(nx, &m, &twopm) || twopm != nx)
        return false;
    for (j = 0; j < ny; j++) {
        for (i = 0; i < nx; i++) {
            real[i] = arr[i][j].real();
            imag[i] = arr[i][j].imag();
        }
        FFT(dir, m, real, imag);
        for (i = 0; i < nx; i++) {
            arr[i][j].real(real[i]);
            arr[i][j].imag(imag[i]);
        }
    }
    free(real);
    free(imag);

    /* Transform the columns */
    real = (double*)malloc(ny * sizeof(double));
    imag = (double*)malloc(ny * sizeof(double));
    if (real == NULL || imag == NULL)
        return false;
    if (!Powerof2(ny, &m, &twopm) || twopm != ny)
        return false;
    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny; j++) {
            real[j] = arr[i][j].real();
            imag[j] = arr[i][j].imag();
        }
        FFT(dir, m, real, imag);
        for (j = 0; j < ny; j++) {
            arr[i][j].real(real[j]);
            arr[i][j].imag(imag[j]);
        }
    }
    free(real);
    free(imag);

    return true;
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader ourShader("C:/Users/Lucas/source/repos/ImageFrequencyAnalysis/src/texture.vert", "C:/Users/Lucas/source/repos/ImageFrequencyAnalysis/src/texture.frag");

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f
    };
    glm::vec3 cubePositions[] = {
        glm::vec3(-1.5f,  0.0f,  0.0f),
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(1.5f,  0.0f,  0.0f)
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int texture1, texture2;
    // texture 1
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("C:/Users/Lucas/source/repos/ImageFrequencyAnalysis/src/building.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    unsigned char* frequency = new unsigned char[width * height * 3];
    complex<double>** arr_r;
    complex<double>** arr_g;
    complex<double>** arr_b;
    arr_r = new complex<double> * [width];
    arr_g = new complex<double> * [width];
    arr_b = new complex<double> * [width];
    for (int i = 0; i < width; i++)
    {
        arr_r[i] = new complex<double>[height];
        arr_g[i] = new complex<double>[height];
        arr_b[i] = new complex<double>[height];
    }
    // Populate arrays
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            arr_r[i][j] = { (double)data[(j * width + i) * 3 + 0], 0 };
            arr_g[i][j] = { (double)data[(j * width + i) * 3 + 1], 0 };
            arr_b[i][j] = { (double)data[(j * width + i) * 3 + 2], 0 };
        }
    }
    // Perform Fast Fourier Transform.
    FFT2D(arr_r, width, height, 1);
    FFT2D(arr_g, width, height, 1);
    FFT2D(arr_b, width, height, 1);
    // Display frequency domain
    const double scale = 255/log(255);
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            int translated_x = (i + width / 2) % width;
            int translated_y = (j + height / 2) % height;
            int unit = (translated_y * width + translated_x) * 3;
            // r
            double real_r = arr_r[i][j].real();
            double imag_r = arr_r[i][j].imag();
            double mag_r = sqrt(real_r*real_r + imag_r*imag_r);
            frequency[unit + 0] = scale * log(1 + mag_r);
            // g
            double real_g = arr_g[i][j].real();
            double imag_g = arr_g[i][j].imag();
            double mag_g = sqrt(real_g * real_g + imag_g * imag_g);
            frequency[unit + 1] = scale * log(1 + mag_g);
            // b
            double real_b = arr_b[i][j].real();
            double imag_b = arr_b[i][j].imag();
            double mag_b = sqrt(real_b * real_b + imag_b * imag_b);
            frequency[unit + 2] = scale * log(1 + mag_b);
        }
    }
    //std::cout << "frequency[0][0] = " << +frequency[500] << '\n';
    //std::cout << "frequency[1][0] = " << +frequency[501] << '\n';
    //std::cout << "frequency[2][0] = " << +frequency[502] << '\n';
    // texture 2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, frequency);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    delete[] frequency;
    

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // activate shader
        ourShader.use();

        // camera/view transformation
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("view", view);

        // render boxes
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 3; i++)
        {
            ourShader.setInt("texture1", i % 2);
            glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            ourShader.setMat4("projection", projection);

            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    cameraPos.x = std::clamp(cameraPos.x, -2.0f, 2.0f);
    cameraPos.y = std::clamp(cameraPos.y, -1.0f, 1.0f);
    cameraPos.z = std::clamp(cameraPos.z, 0.5f, 3.0f);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov = std::clamp(fov - (float)yoffset, 1.0f, 90.0f);
}