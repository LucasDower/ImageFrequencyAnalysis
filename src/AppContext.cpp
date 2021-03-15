#include "AppContext.hpp"

void app_context::load_input_image()
{
	input_image_ = std::make_unique<image_handler>(&filename_buffer_[0]);
	input_image_loaded_ = true;
}

void app_context::show_input_image() const
{
	if (!input_image_loaded_)
	{
		return;
	}
	printf("- show input image\n");

	/*
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Image", nullptr);
	app_context->show_input_image();
	ImGui::End();
	*/
}
