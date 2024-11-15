#include "Texture.hpp"
#include "glad/gl.h"

#include <iostream>
#include <cassert>
#include <vector>
#include <stdint.h>

Texture Texture::Load(const Image& InImage, const Filtering InFiltering)
{
	Texture OutTexture;

	if (InImage.IsValid())
	{
		{
			const GLenum Error = glGetError();
			if (Error != GL_NO_ERROR)
			{
				std::cout << "Pre: " << Error << std::endl;
			}
		}

		glGenTextures(1, &OutTexture.m_TextureId);
		glBindTexture(GL_TEXTURE_2D, OutTexture.m_TextureId);

		const GLint TextureFiltering = InFiltering == Filtering::Linear ? GL_LINEAR : GL_NEAREST;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFiltering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFiltering);

		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

		assert(InImage.GetChannels() == 3 || InImage.GetChannels() == 4);
		const GLint InternalFormat = InImage.GetChannels() == 3 ? GL_RGB32F : GL_RGBA32F;
		const GLint Format = InImage.GetChannels() == 3 ? GL_RGB : GL_RGBA;

		const std::vector<float> FloatImageBuffer(InImage.GetData().begin(), InImage.GetData().end());

		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, InImage.GetWidth(), InImage.GetHeight(), 0, Format, GL_FLOAT, &FloatImageBuffer.front());

		{
			const GLenum Error = glGetError();
			if (Error != GL_NO_ERROR)
			{
				std::cout << "Post: " << Error << std::endl;
			}
		}
	}

	return OutTexture;
}

Texture::Texture()
	: m_TextureId(0)
{
}

Texture::~Texture()
{
	Reset();
}

Texture::Texture(Texture&& Other) noexcept
{
	m_TextureId = Other.m_TextureId;
	Other.m_TextureId = 0;
}

Texture& Texture::operator=(Texture&& Other) noexcept
{
	Reset();

	m_TextureId = Other.m_TextureId;
	Other.m_TextureId = 0;

	return *this;
}

bool Texture::IsValid() const
{
	return m_TextureId != 0;
}

GLuint Texture::GetTextureId() const
{
	return m_TextureId;
}

void Texture::Reset()
{
	if (IsValid())
	{
		glDeleteTextures(1, &m_TextureId);
		m_TextureId = 0;
	}
}
