#include "AppContext.hpp"

#include <stdexcept>
#include <fstream>

app_context::app_context()
{
	filename_buffer_ = std::vector<char>(200, 0);


	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glGenBuffers(1, &vbo_);
	
	float editor_vertices[] = {
		//  Position              Tex-coords
			-0.9f,  0.9f, 0.0f, 0.0f, // Top-left
			 0.9f,  0.9f, 1.0f, 0.0f, // Top-right
			 0.9f, -0.9f,  1.0f, 1.0f, // Bottom-right
			-0.9f, -0.9f, 0.0f, 1.0f  // Bottom-left
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(editor_vertices), editor_vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo_);

	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	const GLchar* vertex_source = R"glsl(
    #version 150 core
    in vec2 position;
    in vec2 texcoord;
    out vec2 Texcoord;
	uniform float aspect;
    void main()
    {
        Texcoord = texcoord;
        gl_Position = vec4(position.x / aspect, position.y, 0.0, 1.0);
    }
	)glsl";
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_source, NULL);
	glCompileShader(vertex_shader);

	const GLchar* fragment_source = R"glsl(
    #version 150 core
    in vec2 Texcoord;
    out vec4 outColor;
    uniform sampler2D tex;
    void main()
    {
        outColor = texture(tex, Texcoord) * vec4(1.0, 1.0, 1.0, 1.0);
    }
	)glsl";
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_source, NULL);
	glCompileShader(fragment_shader);

	// Link the vertex and fragment shader into a shader program
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glBindFragDataLocation(shader_program, 0, "outColor");
	glLinkProgram(shader_program);
	glUseProgram(shader_program);

	// Specify the layout of the vertex data
	const auto pos_attrib = glGetAttribLocation(shader_program, "position");
	glEnableVertexAttribArray(pos_attrib);
	glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	const auto tex_attrib = glGetAttribLocation(shader_program, "texcoord");
	glEnableVertexAttribArray(tex_attrib);
	glVertexAttribPointer(tex_attrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
}

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

	// TODO: Replace with copy constructor
	auto dct_data = input_image_dct_->get_channels();
	auto linear_data = input_image_dct_->get_linear_data();
	mask_ = std::make_unique<masked_image_handler>(std::move(dct_data), linear_data, input_image_->get_width(), input_image_->get_height(), input_image_->get_num_channels());
	mask_state_ = image_state::loaded;
}

void app_context::destroy_buffers()
{
	// TODO: Destroy VAO, VBO, EBO
	input_image_->destroy_buffer();
	input_image_dct_->destroy_buffer();
}

void app_context::handle_editor()
{
	int editor_x, editor_y;
	get_editor_cursor_pos(editor_x, editor_y);
	//printf("%d, %d\n", editor_x, editor_y);
	mask_->set_pixel(editor_x, editor_y, 0, 3);
	mask_->update_texture();
}

void app_context::update_inverse()
{
	auto image = mask_->get_output_image();
	output_image_ = std::make_unique<image_handler>(std::move(image), mask_->get_width(), mask_->get_height(), mask_->get_num_channels());
	output_state_ = image_state::loaded;
}

std::string app_context::get_input_image_error() const
{
	return input_image_error_;
}

void app_context::get_editor_cursor_pos(int& editor_cursor_x, int& editor_cursor_y) const
{
	if (input_image_dct_state_ == image_state::loaded)
	{
		const auto w = static_cast<double>(display_width);
		const auto h = static_cast<double>(display_height);
		auto x = (cursor_x - w / 2.0) / w;
		auto y = (cursor_y - h / 2.0) / h;
		x *= get_aspect_ratio();
		x /= 0.9;
		y /= 0.9;
		const auto im_w = get_input_dct_image()->get_width();
		const auto im_h = get_input_dct_image()->get_height();
		editor_cursor_x = static_cast<int>((x + 0.5) * im_w);
		editor_cursor_y = static_cast<int>((y + 0.5) * im_h);
		return;
	}
	editor_cursor_x = -1;
	editor_cursor_y = -1;
}

ImVec2 app_context::get_max_window_size() const
{
	const auto min_size = static_cast<float>(std::min(display_width, display_height));
	return ImVec2(min_size - 50.0f, min_size - 50.0f);
}

double app_context::get_aspect_ratio() const
{
	return static_cast<double>(display_width) / static_cast<double>(display_height);
}
