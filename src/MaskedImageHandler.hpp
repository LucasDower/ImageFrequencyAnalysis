#ifndef MASKED_IMAGE_HANDLER_H
#define MASKED_IMAGE_HANDLER_H

#pragma once

#include "ImageHandler.hpp"

class masked_image_handler : public image_handler
{
public:
	explicit masked_image_handler(const std::string& filename);
	void set_pixel(int i, int j, unsigned char value) const;
	//void set_mask_overlay(float value01);
	void update_texture() const;
	float mask_overlay{ 0.1f };
private:
	void update_masked_data() const;
	std::shared_ptr<bool[]> mask_{ nullptr };
	std::shared_ptr<unsigned char[]> masked_raw_data_{ nullptr };
};

#endif
