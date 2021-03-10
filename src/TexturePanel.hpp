#include <glad/glad.h>
#include <../../glfw/include/GLFW/glfw3.h>
#include <vector>

class TexturePanel
{
public:
	enum RGBChannel { RED, GREEN, BLUE };

	TexturePanel(float xOffset, float yOffset, float size) : xOffset{ xOffset }, yOffset{ yOffset }, size{ size } {}
	~TexturePanel();
	void SetData(unsigned char* data, int width, int height, int channels);
	void SetImage(const char* filename);
	std::vector<float>& GetChannel(int channel);
	std::vector<float>& GetChannel();
	void DrawPanel();
	void DrawBoundary();

private:
	void BindTexture();
	std::vector<float>& CollectChannel(int channelOffset);
	float xOffset{ 0.0f };
	float yOffset{ 0.0f };
	float size{ 0.1f };
	unsigned char* data{ nullptr };
	bool isDataSet{ false };
	int width{ 0 };
	int height{ 0 };
	int numChannels{ 1 };
	unsigned int handle{ 0 };
};