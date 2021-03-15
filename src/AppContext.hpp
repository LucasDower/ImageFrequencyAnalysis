#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include "ImageHandler.hpp"

#include <memory>
#include <vector>

class app_context
{
public:
	app_context()
	{
		filename_buffer_ = std::vector<char>(50, 0);
		printf("%c\n", filename_buffer_[49]);
	}
	void load_input_image();
	void show_input_image() const;
	[[nodiscard]] bool input_image_loaded() const { return input_image_loaded_; }
	std::vector<char>& get_filename_buffer() { return filename_buffer_; }
	std::shared_ptr<image_handler> get_input_image() const { return input_image_; }
private:
	std::shared_ptr<image_handler> input_image_{ nullptr };
	std::vector<char> filename_buffer_;
	bool input_image_loaded_{ false };
};

#endif