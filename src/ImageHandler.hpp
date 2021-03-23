#ifndef IMAGE_HANDLER_H
#define IMAGE_HANDLER_H

#pragma once

#include <memory>
#include <string>
#include <stdexcept>

class image_handler
{
public:
	image_handler(std::unique_ptr<unsigned char[]> data, int width, int height, int num_channels);
	explicit image_handler(const std::string& filename);
	explicit image_handler(const int width, const int height);
	void get_channel(const int channel, unsigned char*& output) const;
	void get_channel(const int channel, double*& output) const;
	void get_channel(unsigned char*& output) const;
	void get_channel(double*& output) const;
	void apply_dct();
	void collapse_to_greyscale();
	void destroy_buffer() const;
	void use_texture() const;
	[[nodiscard]] bool is_greyscale() const { return num_channels_ == 1; }
	[[nodiscard]] std::unique_ptr<unsigned char[]> get_channels() const;
	[[nodiscard]] int get_width() const { return width_; }
	[[nodiscard]] int get_height() const { return height_; }
	[[nodiscard]] int get_num_channels() const { return num_channels_; }
	[[nodiscard]] unsigned int get_handle() const { return handle_; }
	[[nodiscard]] std::shared_ptr<unsigned char []> get_data() const { return raw_data_; }
	void update_texture() const;
private:
	void bind_texture();
	void collect_channel(const int channel_offset, unsigned char*& output) const;
	void collect_channel(const int channel_offset, double*& output) const;
	void apply_rgb_dct();
	void apply_bw_dct();
	std::shared_ptr<unsigned char[]> raw_data_{ nullptr };
	int width_{ 0 };
	int height_{ 0 };
	int num_channels_{ 1 };
	unsigned int handle_{ 0 };
	
};

#endif
