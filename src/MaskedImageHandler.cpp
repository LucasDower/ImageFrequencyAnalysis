#include "MaskedImageHandler.hpp"

#include "glad/glad.h"
#include <algorithm>

/*
masked_image_handler::masked_image_handler(const std::string& filename)
	: image_handler(filename)
{
	const auto size = static_cast<long long>(get_width()) * static_cast<long long>(get_height());
	mask_ = std::make_unique<bool[]>(size);
	for (auto i = 0; i < size; ++i)
	{
        mask_[i] = true;
	}
	
    masked_raw_data_ = std::make_unique<unsigned char[]>(size); // TODO: Add multi-channel support
    update_masked_data();
}
*/


masked_image_handler::masked_image_handler(std::unique_ptr<unsigned char[]> data, const int width, const int height, const int num_channels)
    : image_handler(std::move(data), width, height, num_channels)
{
    const auto size = static_cast<long long>(width) * static_cast<long long>(height);
    mask_ = std::make_unique<bool[]>(size);
    for (auto i = 0; i < size; ++i)
    {
        mask_[i] = true;
    }

    masked_raw_data_ = std::make_unique<unsigned char[]>(size); // TODO: Add multi-channel support
    update_masked_data();
}

void masked_image_handler::set_pixel(const int i, const int j, const unsigned char value, const unsigned char brush_size) const
{
    const auto raw_data = get_data();
	
    for (auto x = -brush_size; x < brush_size; ++x)
    {
        for (auto y = -brush_size; y < brush_size; ++y)
        {
            if (i + x >= 0 && i + x < get_width() && j + y >= 0 && j + y < get_height())
            {
                const auto index = static_cast<long long>(j+x) * static_cast<long long>(get_width()) + static_cast<long long>(i+y);
                mask_[index] = value;
                //masked_raw_data_[i] = mask_[i] * raw_data[i] + !mask_[i] * static_cast<unsigned char>(static_cast<float>(raw_data[i]) * mask_overlay);
            }
        }
    }
}

/*
void masked_image_handler::set_mask_overlay(const float value01)
{
    mask_overlay_ = std::max(0.0f, std::min(value01, 1.0f));
}
*/

void masked_image_handler::update_texture() const
{
    update_masked_data();
	
    glBindTexture(GL_TEXTURE_2D, get_handle());

    printf("num_channels: %d\n", get_num_channels());
    switch (get_num_channels())
    {
    case 3:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, get_width(), get_height(), 0, GL_RGB, GL_UNSIGNED_BYTE, masked_raw_data_.get());
        break;
    case 1:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, get_width(), get_height(), 0, GL_RED, GL_UNSIGNED_BYTE, masked_raw_data_.get());
        break;
    default:
        throw std::range_error("Invalid number of channels");
    }
}

void masked_image_handler::update_masked_data() const
{
    const auto raw_data = get_data();
	
    const auto size = static_cast<long long>(get_width()) * static_cast<long long>(get_height());
	for (auto i = 0; i < size; ++i)
	{
        masked_raw_data_[i] = mask_[i] * raw_data[i] + !mask_[i] * static_cast<unsigned char>(static_cast<float>(raw_data[i]) * mask_overlay);
	}
}
