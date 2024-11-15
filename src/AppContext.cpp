#include "AppContext.hpp"

#include <stdexcept>
#include <fstream>
#include <iostream>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Image.hpp"
#include "ImageDCT.hpp"

AppContext::AppContext()
	: m_RequestLoadInputImage(false)
{
}

void AppContext::Init()
{
	FilenameBuffer = std::vector<char>(256);
}

bool PendingUpdate = false;

void AppContext::Update()
{
	if (m_RequestLoadInputImage)
	{
		const std::string Filepath(FilenameBuffer.begin(), FilenameBuffer.end());
		m_InputImage = Image::Load(Filepath);

		if (m_InputImage.IsValid())
		{
			m_InputTexture = Texture::Load(m_InputImage, Texture::Filtering::Linear);

			m_DCTImage = ImageDCT::Transform(m_InputImage);
			
			m_DCTNormalisedImage = ImageDCT::NormalizeDCTImage(m_DCTImage);
			m_DCTNormalisedTexture = Texture::Load(m_DCTNormalisedImage, Texture::Filtering::Nearest);

			m_OutputImage = ImageDCT::InverseTransform(m_DCTImage);
			m_OutputTexture = Texture::Load(m_OutputImage, Texture::Filtering::Linear);
		}
		else
		{
			m_InputTexture.Reset();
		}

		m_RequestLoadInputImage = false;
	}

	if (!ImGui::IsMouseDown(0) && PendingUpdate)
	{
		m_DCTNormalisedImage = ImageDCT::NormalizeDCTImage(m_DCTImage);
		m_DCTNormalisedTexture = Texture::Load(m_DCTNormalisedImage, Texture::Filtering::Nearest);

		m_OutputImage = ImageDCT::InverseTransform(m_DCTImage);
		m_OutputTexture = Texture::Load(m_OutputImage, Texture::Filtering::Linear);

		PendingUpdate = false;
	}
}


void AppContext::Render(GLFWwindow& Window)
{
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiViewport* Viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(Viewport->Pos);
	ImGui::SetNextWindowSize(Viewport->Size);
	ImGui::SetNextWindowViewport(Viewport->ID);

	const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;

	ImGui::Begin("MainDockSpace", nullptr, WindowFlags);
	{
		ImGuiID DockSpaceId = ImGui::GetID("MainDockSpace");

		ImGui::DockSpace(DockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	}
	ImGui::End();

	// Draw Input Window
	ImGui::Begin("Input Window", nullptr);
	{
		ImGui::InputText("Filename", &FilenameBuffer.data()[0], FilenameBuffer.size(), 0, nullptr, nullptr);

		if (ImGui::Button("Load"))
		{
			m_RequestLoadInputImage = true;
		}

		if (m_InputImage.IsValid())
		{
			ImGui::Text("Image Loaded!");
		}

		if (m_InputTexture.IsValid())
		{
			ImGui::Image((ImTextureID)(intptr_t)m_InputTexture.GetTextureId(), ImVec2(m_InputImage.GetWidth(), m_InputImage.GetHeight()));
		}

		if (m_OutputTexture.IsValid())
		{
			ImGui::Image((ImTextureID)(intptr_t)m_OutputTexture.GetTextureId(), ImVec2(m_OutputImage.GetWidth(), m_OutputImage.GetHeight()));
		}
	}
	ImGui::End();

	// Draw Input Image
	ImGui::Begin("Input Image", nullptr);
	{
		if (m_DCTNormalisedTexture.IsValid())
		{
			ImVec2 WindowSize = ImGui::GetContentRegionAvail();
			WindowSize.x = std::min(WindowSize.x, WindowSize.y);
			WindowSize.y = WindowSize.x;

			const ImVec2 CursorScreenPos = ImGui::GetCursorScreenPos();
			const ImVec2 MousePos = ImGui::GetMousePos();

			ImVec2 RelativeMousePos = ImVec2((MousePos.x - CursorScreenPos.x) / WindowSize.x, (MousePos.y - CursorScreenPos.y) / WindowSize.y);

			if (ImGui::IsMouseDown(0) && RelativeMousePos.x < 1.0 && RelativeMousePos.x > 0.0 && RelativeMousePos.y < 1.0 && RelativeMousePos.y > 0.0)
			{
				const int32_t X = m_DCTImage.GetWidth() * RelativeMousePos.x;
				const int32_t Y = m_DCTImage.GetHeight() * RelativeMousePos.y;

				for (int32_t ChannelIndex = 0; ChannelIndex < std::min(m_DCTImage.GetChannels(), 3); ++ChannelIndex)
				{
					m_DCTImage.GetMutableData()[m_DCTImage.GetChannels() * (Y * m_DCTImage.GetWidth() + X) + ChannelIndex] = 0;
				}
				PendingUpdate = true;
			}
				
			ImGui::Image((ImTextureID)(intptr_t)m_DCTNormalisedTexture.GetTextureId(), WindowSize);
		}
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}