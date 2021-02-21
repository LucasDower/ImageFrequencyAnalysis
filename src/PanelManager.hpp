#include <glad/glad.h>
#include "../../glfw/include/GLFW/glfw3.h"

enum class Anchor { LEFT, CENTRE, RIGHT };
enum class Sizing { RELATIVE, FILL, ABSOLUTE };
enum class Direction { HORIZONTAL, VERTICAL };

class PanelManager
{
public:
	PanelManager(float viewportWidth, float viewPortHeight, float padding);
	void DrawPanel();
	void SplitPanel(Direction direction, );
	void AddPanel(Anchor anchor, Sizing sizing);

private:
	float aspect = 1.0f;
	float padding = 0.0f;
	float centreX = 0.0f;
	float centreY = 0.0f;
	float widthBy2 = 1.0f;
	float heightBy2 = 1.0f;
	Anchor anchor = Anchor::CENTRE;
	Sizing sizing = Sizing::FILL;
};