#pragma once

#include <imgui.h>
#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include <memory>
#include <vector>
#include "Image.hpp"
#include "Texture.hpp"

class AppContext
{
public:
	AppContext();

	void Init();
	void Update();
	void Render(GLFWwindow& Window);

private:
	bool m_RequestLoadInputImage;

	std::vector<char> FilenameBuffer;

	Image m_InputImage;
	Texture m_InputTexture;

	Image m_DCTImage;

	Image m_DCTNormalisedImage;
	Texture m_DCTNormalisedTexture;

	Image m_OutputImage;
	Texture m_OutputTexture;
};