#include "AppContext.hpp"

#include <stdexcept>
#include <cassert>

void app_context::load_input_image()
{
	try
	{
		input_image_ = std::make_unique<image_handler>(&filename_buffer_[0]);
	}
	catch (std::invalid_argument& ex)
	{
		input_image_error_ = std::string(ex.what());
		input_image_state_ = image_state::failed;
		return;
	}
	
	input_image_state_ = image_state::loaded;
}

void app_context::perform_input_dct()
{
	if (input_image_state_ != image_state::loaded)
	{
		return;
	}
	// TODO: Replace with copy constructor
	auto input_image_data = input_image_->get_channels();
	input_image_dct_ = std::make_unique<image_handler>(std::move(input_image_data), input_image_->get_width(), input_image_->get_height(), input_image_->get_num_channels());
	input_image_dct_state_ = image_state::loaded;

	// TODO: Add2^n exception handling for images not of size 2^n
	input_image_dct_->apply_dct();
}

std::string app_context::get_input_image_error() const
{
	return input_image_error_;
}
