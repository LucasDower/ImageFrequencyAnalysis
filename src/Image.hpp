#pragma once

#include <stdint.h>
#include <vector>
#include <string>

class Image
{
public:
	static Image Create(const int32_t Width, const int32_t Height, const int32_t Channels);
	static Image Load(const std::string& Filepath);

	Image();
	~Image();

	bool IsValid() const;

	int32_t GetWidth() const;
	int32_t GetHeight() const;
	int32_t GetChannels() const;
	size_t GetChannelDataSize() const;
	size_t GetDataSize() const;

	void SetData(const std::vector<double>& Data);
	const std::vector<double>& GetData() const;

	std::vector<double>& GetMutableData();

	void SetChannelData(const uint32_t ChannelIndex, const std::vector<double>& ChannelData);
	void GetChannelData(const uint32_t ChannelIndex, std::vector<double>& OutputBuffer) const;

private:
	int32_t m_Width;
	int32_t m_Height;
	int32_t m_Channels;
	std::vector<double> m_Data;
};