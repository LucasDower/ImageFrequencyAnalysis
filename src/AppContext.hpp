#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include "ImageHandler.hpp"

#include <memory>
#include <vector>

enum class image_state { empty, loaded, failed };

class app_context
{
public:
	// TODO: Allow buffer resizing
	app_context() { filename_buffer_ = std::vector<char>(200, 0); }
	void load_input_image();
	void perform_input_dct();
	[[nodiscard]] image_state get_input_image_state() const { return input_image_state_; }
	[[nodiscard]] std::string get_input_image_error() const;
	[[nodiscard]] image_state get_input_image_dct_state() const { return input_image_dct_state_; }
	std::vector<char>& get_filename_buffer() { return filename_buffer_; }
	[[nodiscard]] std::shared_ptr<image_handler> get_input_image() const { return input_image_; }
	[[nodiscard]] std::shared_ptr<image_handler> get_input_dct_image() const { return input_image_dct_; }
private:
	std::vector<char> filename_buffer_;
	// Input image
	std::shared_ptr<image_handler> input_image_{ nullptr };
	image_state input_image_state_{ image_state::empty };
	std::string input_image_error_;
	// Input image DCT
	std::shared_ptr<image_handler> input_image_dct_{ nullptr };
	image_state input_image_dct_state_{ image_state::empty };
	
};

#endif