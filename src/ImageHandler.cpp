#include "ImageHandler.hpp"
#include "MathUtil.hpp"

#include "glad/glad.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <stdexcept>
#include <algorithm>



image_handler::image_handler(std::unique_ptr<unsigned char[]> data, const int width, const int height, const int num_channels)
{
	raw_data_ = std::move(data);
	width_ = width;
	height_ = height;
	num_channels_ = num_channels;

    bind_texture();
}


image_handler::image_handler(const std::string& filename)
{
    std::unique_ptr<unsigned char[]> stbi_data(stbi_load(filename.c_str(), &width_, &height_, &num_channels_, 0));
    if (!stbi_data || width_ <= 0 || height_ <= 0 || (num_channels_ != 3 && num_channels_ != 1))
    {
        throw std::invalid_argument("Bad image");
    }
    raw_data_ = std::move(stbi_data);

    bind_texture();
}

image_handler::image_handler(const int width, const int height)
{
    width_ = width;
    height_ = height;
    num_channels_ = 1;
    const auto size = static_cast<long long>(width) * static_cast<long long>(width);
    raw_data_ = std::make_unique<unsigned char[]>(size);
    for (auto i = 0; i < size; ++i)
    {
        raw_data_[i] = 255;
    }

    bind_texture();
}

void image_handler::bind_texture()
{
	// Delete old texture handle
    if (handle_ != 0)
    {
        printf("Deleting old handle\n");
        glDeleteTextures(1, &handle_);
    }
	
    glGenTextures(1, &handle_);
    glBindTexture(GL_TEXTURE_2D, handle_);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLint rgb_swizzle_mask[] = { GL_RED, GL_GREEN, GL_BLUE, GL_ONE };
    GLint bw_swizzle_mask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
	
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, (num_channels_ == 1) ? bw_swizzle_mask : rgb_swizzle_mask);
	
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    update_texture();
}






void image_handler::get_channel(const int channel, unsigned char*& output) const
{
    collect_channel(channel, output);
}


void image_handler::get_channel(const int channel, double*& output) const
{
    collect_channel(channel, output);
}


void image_handler::get_channel(unsigned char*& output) const
{
    if (num_channels_ != 1)
        throw std::invalid_argument("Ambiguous channel. Use GetChannel(RGBChannel channel) for non-greyscale images.");
    collect_channel(0, output);
}


void image_handler::get_channel(double*& output) const
{
    if (num_channels_ != 1)
        throw std::invalid_argument("Ambiguous channel. Use GetChannel(RGBChannel channel) for non-greyscale images.");
    collect_channel(0, output);
}


std::unique_ptr<unsigned char[]> image_handler::get_channels() const
{
    const auto size = static_cast<long long>(width_) * static_cast<long long>(height_) * static_cast<long long>(num_channels_);
    auto output = std::make_unique<unsigned char[]>(size);
    for (auto i = 0; i < size; ++i)
    {
        output[i] = raw_data_.get()[i];
    }
    return output;
}


// TODO: Replace with smart pointers
void image_handler::collect_channel(const int channel_offset, unsigned char*& output) const
{
    if (channel_offset >= num_channels_)
        throw std::range_error("Channel out of range.");

    const auto size = static_cast<long long>(width_) * static_cast<long long>(height_);
    output = new unsigned char[size];
    for (auto i = 0; i < size; ++i)
    {
        output[i] = raw_data_.get()[i];
    }
}


// TODO: Replace with smart pointers
void image_handler::collect_channel(const int channel_offset, double*& output) const
{
    if (channel_offset >= num_channels_)
        throw std::range_error("Channel out of range.");

    const auto size = static_cast<long long>(width_) * static_cast<long long>(height_);
    output = new double[size];
    for (auto i = 0; i < size; ++i)
    {
        output[i] = static_cast<double>(raw_data_.get()[num_channels_ * i + channel_offset]);
    }
}

void image_handler::apply_dct()
{
	if (num_channels_ == 1)
	{
        apply_bw_dct();
        return;
	}
    apply_rgb_dct();
}


void image_handler::update_texture() const
{
    glBindTexture(GL_TEXTURE_2D, get_handle());

    switch (get_num_channels())
    {
    case 3:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, get_width(), get_height(), 0, GL_RGB, GL_UNSIGNED_BYTE, raw_data_.get());
        break;
    case 1:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, get_width(), get_height(), 0, GL_RED, GL_UNSIGNED_BYTE, raw_data_.get());
        break;
    default:
        throw std::range_error("Invalid number of channels");
    }
}


void image_handler::apply_bw_dct()
{
    // TODO: Replace with smart pointers
    double* depth{ nullptr };

    collect_channel(0, depth);

    math_util::dct_2d(depth, width_, height_, true);

    auto max = std::numeric_limits<double>::min();
    for (auto i = 0; i < width_ * height_; ++i)
    {
        depth[i] = log(1 + abs(depth[i]));
        max = std::max(max, depth[i]);
    }

    const auto scale_factor = 255.0 / max;

    const auto size = static_cast<long long>(width_) * static_cast<long long>(height_);
    auto new_data = std::make_unique<unsigned char[]>(size);
    for (auto i = 0; i < size; ++i)
    {
        new_data[i] = static_cast<unsigned char>(depth[i] * scale_factor);
    }
    raw_data_ = std::move(new_data);

    delete[] depth;

    update_texture();
}


void image_handler::apply_rgb_dct()
{
	// TODO: Replace with smart pointers
    double* red{ nullptr };
    double* green{ nullptr };
    double* blue{ nullptr };
	
    collect_channel(0, red);
    collect_channel(1, green);
    collect_channel(2, blue);
	
    math_util::dct_2d(red, width_, height_, true);
    math_util::dct_2d(green, width_, height_, true);
    math_util::dct_2d(blue, width_, height_, true);

    auto max = std::numeric_limits<double>::min();
    for (auto i = 0; i < width_ * height_; ++i)
    {
        red[i] = log(1 + abs(red[i]));
        green[i] = log(1 + abs(green[i]));
        blue[i] = log(1 + abs(blue[i]));
        max = std::max(max, red[i]);
        max = std::max(max, green[i]);
        max = std::max(max, blue[i]);
    }

    const auto scale_factor = 255.0 / max;

    const auto size = static_cast<long long>(width_) * static_cast<long long>(height_) * static_cast<long long>(num_channels_);
    auto new_data = std::make_unique<unsigned char[]>(size);
    for (auto i = 0; i < static_cast<long long>(width_) * static_cast<long long>(height_); ++i)
    {
        new_data[3LL * i] = static_cast<unsigned char>(red[i] * scale_factor);
        new_data[3LL * i + 1] = static_cast<unsigned char>(green[i] * scale_factor);
        new_data[3LL * i + 2] = static_cast<unsigned char>(blue[i] * scale_factor);
    }
    raw_data_ = std::move(new_data);

    delete[] red;
    delete[] green;
	delete[] blue;
	
    update_texture();
}

void image_handler::collapse_to_greyscale()
{
    printf("Collapse to greyscale\n");

    const auto size = static_cast<long long>(width_) * static_cast<long long>(height_);
    auto new_data = std::make_unique<unsigned char[]>(size);
    for (auto i = 0; i < size; ++i)
    {
        new_data[i] = math_util::convert_rgb_to_bw(raw_data_[3LL * i], raw_data_[3LL * i + 1], raw_data_[3LL * i + 2]);
    }
	
    raw_data_ = std::move(new_data);
    num_channels_ = 1;
	
    bind_texture();
}


void image_handler::destroy_buffer() const
{
    glDeleteTextures(1, &handle_);
}


void image_handler::use_texture() const
{
    glBindTexture(GL_TEXTURE_2D, handle_);
}