#pragma once

namespace ImageHandler
{
	unsigned int loadAndBindTexture(unsigned char** image_buffer, const char* filename, int* image_width, int* image_height);
	unsigned int bindTexture(unsigned char* data, int image_width, int image_height);
}

