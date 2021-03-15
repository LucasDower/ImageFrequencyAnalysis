#ifndef IMAGE_HANDLER_H
#define IMAGE_HANDLER_H

#include <memory>
#include <string>
#include <stdexcept>

class image_handler
{
public:
	image_handler(std::unique_ptr<unsigned char>, int width, int height, int channels);
	explicit image_handler(const std::string& filename);
	void get_channel(const int channel, unsigned char*& output) const;
	void get_channel(const int channel, double*& output) const;
	void get_channel(unsigned char*& output) const;
	void get_channel(double*& output) const;
	[[nodiscard]] int get_width() const { return width_; }
	[[nodiscard]] int get_height() const { return height_; }
	[[nodiscard]] unsigned int get_handle() const { return handle_; }
private:
	void bind_texture();
	void collect_channel(const int channel_offset, unsigned char*& output) const;
	void collect_channel(const int channel_offset, double*& output) const;
	std::unique_ptr<unsigned char> data_{ nullptr };
	int width_{ 0 };
	int height_{ 0 };
	int num_channels_{ 1 };
	unsigned int handle_{ 0 };
};

#endif
