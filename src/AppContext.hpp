#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include "ImageHandler.hpp"
#include "imgui/imgui.h"
#include <glad/glad.h>

#include <memory>
#include <vector>


enum class image_state { empty, loaded, failed };

class app_context
{
public:
	// TODO: Allow buffer resizing
	app_context();
	void load_input_image();
	void perform_input_dct();
	void destroy_buffers();
	[[nodiscard]] float app_context::get_aspect_ratio() const;
	[[nodiscard]] image_state get_input_image_state() const { return input_image_state_; }
	[[nodiscard]] std::string get_input_image_error() const;
	[[nodiscard]] image_state get_input_image_dct_state() const { return input_image_dct_state_; }
	std::vector<char>& get_filename_buffer() { return filename_buffer_; }
	[[nodiscard]] std::shared_ptr<image_handler> get_input_image() const { return input_image_; }
	[[nodiscard]] std::shared_ptr<image_handler> get_input_dct_image() const { return input_image_dct_; }
	bool is_input_greyscale{ false };
	bool draw_editing_window{ false };
	int display_width = 1280;
	int display_height = 720;
	[[nodiscard]] ImVec2 get_max_window_size() const;
	GLuint shader_program{ 0 };
private:
	std::vector<char> filename_buffer_;
	// Input image
	std::shared_ptr<image_handler> input_image_{ nullptr };
	image_state input_image_state_{ image_state::empty };
	std::string input_image_error_;
	// Input image DCT
	std::shared_ptr<image_handler> input_image_dct_{ nullptr };
	image_state input_image_dct_state_{ image_state::empty };
	GLuint vao_{ 0 };
	GLuint vbo_{ 0 };
	GLuint ebo_{ 0 };
};

#endif