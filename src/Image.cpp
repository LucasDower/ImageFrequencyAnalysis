#include "Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "External/stb_image.h"

#include <algorithm>

Image::Image()
	: m_Width(0)
	, m_Height(0)
	, m_Channels(0)
{
}

Image::~Image()
{
}

Image Image::Create(const int32_t Width, const int32_t Height, const int32_t Channels)
{
	Image OutImage;

	OutImage.m_Width = Width;
	OutImage.m_Height = Height;
	OutImage.m_Channels = Channels;
	OutImage.m_Data.resize(OutImage.GetDataSize());

	if (Channels == 4)
	{
		std::vector<double> AlphaChannel;
		AlphaChannel.resize(OutImage.GetChannelDataSize());
		std::fill(AlphaChannel.begin(), AlphaChannel.end(), 1.0);
		OutImage.SetChannelData(3, AlphaChannel);
	}

	return OutImage;
}

Image Image::Load(const std::string& Filepath)
{
	Image LoadedImage;

	stbi_uc* ImageData = stbi_load(Filepath.c_str(), &LoadedImage.m_Width, &LoadedImage.m_Height, &LoadedImage.m_Channels, 0);
	
	const size_t DataSize = LoadedImage.GetDataSize();
	for (size_t Index = 0; Index < DataSize; ++Index) {
		LoadedImage.m_Data.push_back(static_cast<double>(ImageData[Index]) / 255.0);
	}

	stbi_image_free(ImageData);

	return LoadedImage;
}

bool Image::IsValid() const
{
	return m_Width > 0 && m_Height > 0 && (m_Channels == 3 || m_Channels == 4);
}

int32_t Image::GetWidth() const
{
	return m_Width;
}

int32_t Image::GetHeight() const
{
	return m_Height;
}

int32_t Image::GetChannels() const
{
	return m_Channels;
}

std::vector<double>& Image::GetMutableData()
{
	return m_Data;
}

const std::vector<double>& Image::GetData() const
{
	return m_Data;
}

size_t Image::GetChannelDataSize() const
{
	return m_Width * m_Height;
}

size_t Image::GetDataSize() const
{
	return m_Width * m_Height * m_Channels;
}

void Image::SetChannelData(const uint32_t ChannelIndex, const std::vector<double>& ChannelData)
{
	assert(ChannelIndex < m_Channels);
	assert(ChannelData.size() == m_Width * m_Height);

	for (uint32_t Index = 0; Index < m_Width * m_Height; ++Index)
	{
		m_Data[Index * m_Channels + ChannelIndex] = ChannelData[Index];
	}
}

void Image::GetChannelData(const uint32_t ChannelIndex, std::vector<double>& OutputBuffer) const
{
	assert(ChannelIndex < m_Channels);

	OutputBuffer.resize(GetChannelDataSize());

	for (uint32_t Index = 0; Index < m_Width * m_Height; ++Index)
	{
		OutputBuffer[Index] = m_Data[Index * m_Channels + ChannelIndex];
	}
}

void Image::SetData(const std::vector<double>& Data)
{
	m_Data = Data;
}