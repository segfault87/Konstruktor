#include <cstdio>
#include <cstring>
#include <iostream>

#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include <renderer/renderer_opengl.h>

#include "modelviewer.h"

int frames_ = 0, timebase_ = 0, fps_ = 0;

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

bool initializeVideo(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("libLDR Renderer Test");

	return true;
}

void renderText(int x, int y, const unsigned char *text)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glRasterPos2i(x, y);
	glutBitmapString(GLUT_BITMAP_HELVETICA_10, text);

	glPopMatrix();
}

void displayFunc()
{
	int start, end;
	char text[100];
	
	start = glutGet(GLUT_ELAPSED_TIME);
	
	render(start);

	end = glutGet(GLUT_ELAPSED_TIME);

	++frames_;
	if (end - timebase_ > 1000) {
		fps_ = frames_ * 1000.0 / (end - timebase_);
		timebase_ = end;
		frames_ = 0;
	}

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, width_, height_, 0.0, 0.0, 100.0);

	std::snprintf(text, 50, "%d fps", fps_);
	renderText(10, 35, (const unsigned char *)text);

	std::snprintf(text, 50, "%d msec(s)", end - start);
	renderText(10, 20, (const unsigned char *)text);

	if (mode_ == ldraw_renderer::renderer_opengl_factory::mode_immediate)
		std::snprintf(text, sizeof(text), "%s", msg_immediate);
	else
		std::snprintf(text, sizeof(text), msg_retained,
					  mode_ == ldraw_renderer::renderer_opengl_factory::mode_varray ? msg_varray : msg_vbo,
					  params_.get_shader() ? msg_shader : msg_no_shader);

	renderText(10, 50, (const unsigned char *)text);

	if (mode_ != ldraw_renderer::renderer_opengl_factory::mode_immediate) {
		std::snprintf(text, sizeof(text), "%.3f Kbytes of vertex buffers", memsiz_ / 1024.0f);
		renderText(10, 65, (const unsigned char *)text);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	if (!initializeLdraw())
		return -1;
	if (!initializeVideo(argc, argv))
		return -1;
	
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " [filename] (-immediate | -varray | -vbo)" << std::endl;
		return -2;
	} else if (argc > 2) {
		if (std::strcmp(argv[2], "-immediate") == 0)
			mode_ = ldraw_renderer::renderer_opengl_factory::mode_immediate;
		else if (std::strcmp(argv[2], "-varray") == 0)
			mode_ = ldraw_renderer::renderer_opengl_factory::mode_varray;
		else if (std::strcmp(argv[2], "-vbo") == 0)
			mode_ = ldraw_renderer::renderer_opengl_factory::mode_vbo;
		else {
			std::cerr << "invalid option." << std::endl;
			return -3;
		}
	}

	initDisplay();
	
	if (!initializeModel(argv[1]))
		return -1;

	resize(SCREEN_WIDTH, SCREEN_HEIGHT);	
	
	glutDisplayFunc(displayFunc);
	glutIdleFunc(displayFunc);
	glutReshapeFunc(resize);
	glutMainLoop();

	return 0;
}
