#include "TexturePanel.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <../../stb_image.h>
#include <iostream>
#include <stdexcept>


TexturePanel::~TexturePanel()
{
    stbi_image_free(data);
}

void TexturePanel::SetData(unsigned char* _data, int _width, int _height, int _channels)
{
	data = _data;
	width = _width;
	height = _height;
	numChannels = _channels;

    BindTexture();
}


void TexturePanel::SetImage(const char* filename)
{
    data = stbi_load(filename, &width, &height, &numChannels, 0);
    isDataSet = data != nullptr;

    BindTexture();
}


void TexturePanel::BindTexture()
{
    glGenTextures(1, &handle);

    glBindTexture(GL_TEXTURE_2D, handle);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    isDataSet = true;
}


std::vector<float>& TexturePanel::GetChannel(int channel)
{
    return CollectChannel(channel);
}


std::vector<float>& TexturePanel::GetChannel()
{
    if (numChannels != 1)
        throw std::invalid_argument("Ambiguous channel. Use GetChannel(RGBChannel channel) for non-greyscale images.");
    return CollectChannel(0);
}


std::vector<float>& TexturePanel::CollectChannel(int channelOffset)
{
    if (channelOffset >= numChannels)
        throw std::range_error("Channel out of range.");

    std::vector<float> channel;
    for (int i = channelOffset; i < width * height * numChannels; i += numChannels)
    {
        channel.push_back((float) data[i]);
    }
    return channel;
}





void TexturePanel::DrawPanel()
{
    if (!isDataSet)
        return;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    glTranslatef(xOffset, yOffset, 0.0f);
        glBegin(GL_QUADS);
            glVertex2f(-size, -size);
            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(size, -size);
            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(size, size);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(-size, size);
            glTexCoord2f(0.0f, 1.0f);
        glEnd();
    glPopMatrix();
}


void TexturePanel::DrawBoundary()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_TEXTURE_2D);

    glPushMatrix();
        glTranslatef(xOffset, yOffset, 0.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(-size, -size);
            glVertex2f(size, -size);
            glVertex2f(size, size);
            glVertex2f(-size, size);
        glEnd();
    glPopMatrix();
}