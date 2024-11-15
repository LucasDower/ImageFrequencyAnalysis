#include "ImageDCT.hpp"
#include "External/FastDCTLee.hpp"
#include "cassert"
#include <cmath>
#include <iostream>

bool IsPowerOf2(const int32_t Value)
{
	return (Value & (Value - 1)) == 0;
}

void FastDCT2D(std::vector<double>& image, std::vector<double>& dct_result, int width, int height) {
	int N = height;        // Height of the image
	int M = width;     // Width of the image

	// Apply 1D DCT on each row
	for (int i = 0; i < N; ++i) {
		std::vector<double> row(M);
		for (int j = 0; j < M; ++j) {
			row[j] = image[j * M + i];
		}
		{
			FastDctLee::transform(row);
		}
		for (int j = 0; j < M; ++j) {
			dct_result[j * M + i] = row[j];
		}
	}

	// Apply 1D DCT on each column
	for (int j = 0; j < M; ++j) {
		std::vector<double> col(N);
		for (int i = 0; i < N; ++i) {
			col[i] = dct_result[j * M + i];
		}
		{
			FastDctLee::transform(col);
		}
		for (int i = 0; i < N; ++i) {
			dct_result[j * M + i] = col[i];
		}
	}
}

void FastDCT2DInverse(std::vector<double>& image, std::vector<double>& dct_result, int width, int height) {
	int N = height;        // Height of the image
	int M = width;     // Width of the image

	// Apply 1D DCT on each row
	for (int i = 0; i < N; ++i) {
		std::vector<double> row(M);
		for (int j = 0; j < M; ++j) {
			row[j] = image[j * M + i];
		}
		{
			FastDctLee::inverseTransform(row);
			for (double& x : row)
			{
				x /= M / 2.0;
			}
		}
		for (int j = 0; j < M; ++j) {
			dct_result[j * M + i] = row[j];
		}
	}


	// Apply 1D DCT on each column
	for (int j = 0; j < M; ++j) {
		std::vector<double> col(N);
		for (int i = 0; i < N; ++i) {
			col[i] = dct_result[j * M + i];
		}
		{
			FastDctLee::inverseTransform(col);
			for (double& x : col)
			{
				x /= N / 2.0;
			}
		}
		for (int i = 0; i < N; ++i) {
			dct_result[j * M + i] = col[i];
		}
	}


}

Image ImageDCT::Transform(const Image& InImage)
{
	assert(IsPowerOf2(InImage.GetWidth()) && IsPowerOf2(InImage.GetHeight()));

	Image OutImage = Image::Create(InImage.GetWidth(), InImage.GetHeight(), InImage.GetChannels());

	std::vector<double> InputData;
	InputData.resize(InImage.GetChannelDataSize());

	std::vector<double> OutputData;
	OutputData.resize(InImage.GetChannelDataSize());

	// Get red channel
	if (InImage.GetChannels() >= 1)
	{
		InImage.GetChannelData(0, InputData);

		FastDCT2D(InputData, OutputData, InImage.GetWidth(), InImage.GetHeight());

		OutImage.SetChannelData(0, OutputData);
	}

	// Get green channel
	if (InImage.GetChannels() >= 2)
	{
		InImage.GetChannelData(1, InputData);

		FastDCT2D(InputData, OutputData, InImage.GetWidth(), InImage.GetHeight());

		OutImage.SetChannelData(1, OutputData);
	}

	// Get blue channel
	if (InImage.GetChannels() >= 3)
	{
		InImage.GetChannelData(2, InputData);

		FastDCT2D(InputData, OutputData, InImage.GetWidth(), InImage.GetHeight());

		OutImage.SetChannelData(2, OutputData);
	}

	return OutImage;
}

Image ImageDCT::InverseTransform(const Image& InImage)
{
	assert(IsPowerOf2(InImage.GetWidth()) && IsPowerOf2(InImage.GetHeight()));

	Image OutImage = Image::Create(InImage.GetWidth(), InImage.GetHeight(), InImage.GetChannels());

	std::vector<double> InputData;
	InputData.resize(InImage.GetChannelDataSize());

	std::vector<double> OutputData;
	OutputData.resize(InImage.GetChannelDataSize());

	// Get red channel
	if (InImage.GetChannels() >= 1)
	{
		InImage.GetChannelData(0, InputData);

		FastDCT2DInverse(InputData, OutputData, InImage.GetWidth(), InImage.GetHeight());

		OutImage.SetChannelData(0, OutputData);
	}

	// Get green channel
	if (InImage.GetChannels() >= 2)
	{
		InImage.GetChannelData(1, InputData);

		FastDCT2DInverse(InputData, OutputData, InImage.GetWidth(), InImage.GetHeight());

		OutImage.SetChannelData(1, OutputData);
	}

	// Get blue channel
	if (InImage.GetChannels() >= 3)
	{
		InImage.GetChannelData(2, InputData);

		FastDCT2DInverse(InputData, OutputData, InImage.GetWidth(), InImage.GetHeight());

		OutImage.SetChannelData(2, OutputData);
	}

	return OutImage;
}

Image ImageDCT::NormalizeDCTImage(const Image& InImage)
{
	Image OutImage = Image::Create(InImage.GetWidth(), InImage.GetHeight(), InImage.GetChannels());

	std::vector<double> ChannelData;
	ChannelData.resize(InImage.GetChannelDataSize());

	// Red channel
	if (InImage.GetChannels() >= 1)
	{
		InImage.GetChannelData(0, ChannelData);

		double MaxAbs = 0.0;
		for (double Value : ChannelData)
		{
			MaxAbs = std::max(MaxAbs, std::abs(Value));
		}

		for (double& Value : ChannelData) {
			Value = std::log(1 + std::abs(Value)) / std::log(1 + MaxAbs);
		}

		OutImage.SetChannelData(0, ChannelData);
	}

	// Green channel
	if (InImage.GetChannels() >= 2)
	{
		InImage.GetChannelData(1, ChannelData);

		double MaxAbs = 0.0;
		for (double Value : ChannelData)
		{
			MaxAbs = std::max(MaxAbs, std::abs(Value));
		}

		for (double& Value : ChannelData) {
			Value = std::log(1 + std::abs(Value)) / std::log(1 + MaxAbs);
		}

		OutImage.SetChannelData(1, ChannelData);
	}

	// Blue channel
	if (InImage.GetChannels() >= 3)
	{
		InImage.GetChannelData(2, ChannelData);

		double MaxAbs = 0.0;
		for (double Value : ChannelData)
		{
			MaxAbs = std::max(MaxAbs, std::abs(Value));
		}

		for (double& Value : ChannelData) {
			Value = std::log(1 + std::abs(Value)) / std::log(1 + MaxAbs);
		}

		OutImage.SetChannelData(2, ChannelData);
	}

	return OutImage;
}