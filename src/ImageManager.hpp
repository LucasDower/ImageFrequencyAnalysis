#pragma once

unsigned int load_and_bind_texture(unsigned char** image_buffer, const char* filename, int* image_width, int* image_height);
unsigned int bind_texture(unsigned char* data, int image_width, int image_height);