#include "AppContext.hpp"

#include <stdexcept>

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

std::string app_context::get_input_image_error() const
{
	return input_image_error_;
}
