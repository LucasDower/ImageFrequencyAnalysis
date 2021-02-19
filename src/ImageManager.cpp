#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <../../stb_image.h>
#include <stdexcept>

unsigned int bind_texture(unsigned char* data, int image_width, int image_height)
{
    unsigned int tex_handle = 0;

    // request one texture handle
    glGenTextures(1, &tex_handle);

    // create a new texture object and bind it to tex_handle
    glBindTexture(GL_TEXTURE_2D, tex_handle);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    return tex_handle;
}

unsigned int load_and_bind_texture(unsigned char** image_buffer, const char* filename, int* image_width, int* image_height)
{
    int nrChannels = 0;

    // read in the PNG image data into image_buffer
    *image_buffer = stbi_load(filename, image_width, image_height, &nrChannels, 0);

    if (!image_buffer)
    {
        throw std::invalid_argument("Failed to read image texture");
    }

    return bind_texture(*image_buffer, *image_width, *image_height);
}