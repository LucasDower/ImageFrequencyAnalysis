#pragma once

#include "Image.hpp"

namespace ImageDCT
{
	Image Transform(const Image& InImage);

	Image InverseTransform(const Image& InImage);

	Image NormalizeDCTImage(const Image& InImage);
}