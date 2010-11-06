#include <cstdio>
#include <cmath>
#include <iostream>

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include <libldr/bfc.h>
#include <libldr/color.h>
#include <libldr/metrics.h>
#include <libldr/model.h>
#include <libldr/part_library.h>
#include <libldr/reader.h>
#include <renderer/renderer_opengl.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

ldraw::part_library *library_ = 0L;
ldraw::model_multipart *model_ = 0L;
renderer_opengl *renderer_;
int width_, height_;
float length_;
int frames_ = 0, timebase_ = 0, fps_ = 0;

bool initializeLdraw()
{
	ldraw::color::init();

	try {
		library_ = new ldraw::part_library;
	} catch (const ldraw::exception &) {
		std::cerr << "could not load ldraw part library." << std::endl;
		return false;
	}

	std::cerr << "found ldraw: " << library_->ldrawpath() << std::endl;

	return true;
}

bool initializeVideo(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("libLDR Renderer Test");

	return true;
}

bool initializeModel(const char *filename)
{
	ldraw::reader reader;

	model_ = reader.load_from_file(filename);

	if (!model_) {
		std::cerr << "could not read model file: " << filename << std::endl;
		return false;
	}

	library_->link(model_);

	model_->main_model()->update_custom_data<ldraw::metrics>();

	return true;
}

void initDisplay()
{
	glClearColor(0.2, 0.2, 0.2, 1.0);

	renderer_ = new renderer_opengl();
	renderer_->set_shading(true);
	//renderer_->set_debug(true);
	renderer_->setup();
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
	char text[50];
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	start = glutGet(GLUT_ELAPSED_TIME);

	glMatrixMode(GL_MODELVIEW);
	
	glPushMatrix();
	glLoadIdentity();

	GLfloat degrees = start / 3000.0 * 90.0;
	GLfloat position[] = {0.0f, -10000.0f, 0.0f, 1.0f};
	position[0] = std::sin(-degrees / 180.0 * M_PI) * 5000.0;
	position[2] = std::cos(-degrees / 180.0 * M_PI) * 5000.0;

	//glLightfv(GL_LIGHT1, GL_POSITION, position);
	
	glRotatef(degrees, 0.0, 1.0, 0.0);

	renderer_->draw_model(model_->main_model(), std::set<int>());

	glPopMatrix();

	end = glutGet(GLUT_ELAPSED_TIME);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, width_, height_, 0.0, 0.0, 100.0);

	++frames_;
	if (end - timebase_ > 1000) {
		fps_ = frames_ * 1000.0 / (end - timebase_);
		timebase_ = end;
		frames_ = 0;
	}

	glDisable(GL_LIGHTING);
	glColor3ub(255, 255, 255);

	std::snprintf(text, 50, "%d fps", fps_);
	renderText(10, 35, (const unsigned char *)text);

	std::snprintf(text, 50, "%d msec(s)", end - start);
	renderText(10, 20, (const unsigned char *)text);

	const statistics *stats = renderer_->get_stats();

	std::snprintf(text, 50, "%d faces, %d lines", stats->faces, stats->lines);
	renderText(10, 50, (const unsigned char *)text);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glutSwapBuffers();
}

void resizeFunc(int width, int height)
{
	glViewport(0, 0, width, height);
	width_ = width, height_ = height;
	length_ = std::sqrt(std::pow(width_, 2.0) + std::pow(height_, 2.0));

	const ldraw::metrics *metrics = model_->main_model()->custom_data<ldraw::metrics>();
	float distance = ldraw::vector::distance(ldraw::vector(0.0, 0.0, 0.0), (metrics->max() - metrics->min()) * 0.5f) * 1.5f;
	float theight = std::fabs(metrics->max().z() - metrics->min().z());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	ldraw::vector lv = (metrics->min() + metrics->max()) * 0.5f;
	glTranslatef(-lv.x(), -lv.y(), -lv.z());
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)width_ / height_, 0.1, 100000.0);
	gluLookAt(0.0, -theight, distance, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0);
}

int main(int argc, char *argv[])
{
	if (!initializeLdraw())
		return -1;
	if (!initializeVideo(argc, argv))
		return -1;

	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " [filename]" << std::endl;
		return -2;
	}

	initDisplay();
	
	if (!initializeModel(argv[1]))
		return -1;

	resizeFunc(SCREEN_WIDTH, SCREEN_HEIGHT);
	
	glutDisplayFunc(displayFunc);
	glutIdleFunc(displayFunc);
	glutReshapeFunc(resizeFunc);
	glutMainLoop();

	return 0;
}

