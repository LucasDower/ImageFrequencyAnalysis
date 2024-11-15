#pragma once

#include "Image.hpp"
#include "glad/gl.h"

class Texture
{
public:
	enum class Filtering
	{
		Linear,
		Nearest
	};

	static Texture Load(const Image& InImage, const Filtering InFiltering);

	Texture();
	~Texture();

	// Delete copying
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	// Custom move
	Texture(Texture&&) noexcept;
	Texture& operator=(Texture&&) noexcept;

	bool IsValid() const;

	void Reset();

	GLuint GetTextureId() const;

private:
	GLuint m_TextureId;
};