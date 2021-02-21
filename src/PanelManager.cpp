#include "PanelManager.hpp"

PanelManager::PanelManager(float viewportWidth, float viewPortHeight, float padding)
{
	this->aspect = viewportWidth / viewPortHeight;
	this->padding = padding;
}

void PanelManager::DrawPanel()
{
	glPushMatrix();
	//glTranslatef(centreX, centreY, 0.0f);
		glBegin(GL_LINE_LOOP);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2f(-widthBy2 + padding, -heightBy2 + padding * aspect);
			glVertex2f(widthBy2 - padding, -heightBy2 + padding * aspect);
			glVertex2f(widthBy2 - padding, heightBy2 - padding * aspect);
			glVertex2f(-widthBy2 + padding, heightBy2 - padding * aspect);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2f(-0.3f, -0.3f);
			glVertex2f(0.3f, -0.3f);
			glVertex2f(0.3f, 0.3f);
			glVertex2f(-0.3f, 0.3f);
		glEnd();
		
	glPopMatrix();
}

void PanelManager::SplitPanel