#include <glad/glad.h>
#include <../../glfw/include/GLFW/glfw3.h>

class TexturePanel
{
public:
	TexturePanel(float xOffset, float yOffset, float size) : xOffset{ xOffset }, yOffset{ yOffset }, size{ size } {}
	~TexturePanel();
	void SetData(unsigned char* data, int width, int height, int channels);
	void SetImage(const char* filename);
	void DrawPanel();
	void DrawBoundary();

private:
	void BindTexture();
	float xOffset{ 0.0f };
	float yOffset{ 0.0f };
	float size{ 0.1f };
	unsigned char* data{ nullptr };
	bool isDataSet{ false };
	int width{ 0 };
	int height{ 0 };
	int channels{ 1 };
	unsigned int handle{ 0 };
};