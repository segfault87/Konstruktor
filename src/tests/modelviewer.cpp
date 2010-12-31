#include <cmath>
#include <iostream>

#include <GL/gl.h>
#include <GL/glu.h>

#include <libldr/bfc.h>
#include <libldr/color.h>
#include <libldr/metrics.h>
#include <libldr/model.h>
#include <libldr/part_library.h>
#include <libldr/reader.h>
#include <libldr/utils.h>

#include <renderer/normal_extension.h>
#include <renderer/opengl_extension_vbo.h>
#include <renderer/vbuffer_extension.h>

#include "modelviewer.h"

const char *msg_immediate = "OpenGL immediate mode (DEPRECATED)";
const char *msg_retained = "OpenGL retained mode (%s%s)";
const char *msg_varray = "vertex array";
const char *msg_vbo = "vertex buffer objects";
const char *msg_shader = " w/ vertex shader";
const char *msg_no_shader = " wo/ vertex shader";

ldraw::part_library *library_ = 0L;
ldraw::model_multipart *model_ = 0L;
ldraw_renderer::renderer_opengl *renderer_;
int width_, height_;
float length_;
int memsiz_ = 0;
ldraw_renderer::parameters params_;
ldraw_renderer::renderer_opengl_factory::rendering_mode mode_ = ldraw_renderer::renderer_opengl_factory::mode_vbo;

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

bool initializeModel(const char *filename)
{
	ldraw::reader reader;

	model_ = reader.load_from_file(filename);

	if (!model_) {
		std::cerr << "could not read model file: " << filename << std::endl;
		return false;
	}

	library_->link(model_);

	ldraw::utils::validate_bowtie_quads(model_->main_model());

	model_->main_model()->update_custom_data<ldraw::metrics>();

	return true;
}

void initDisplay()
{
	glClearColor(0.2, 0.2, 0.2, 1.0);
	
	params_.set_shading(true);
	params_.set_shader(false);
	params_.set_stud_rendering_mode(ldraw_renderer::parameters::stud_regular);
	params_.set_vbuffer_criteria(ldraw_renderer::parameters::vbuffer_parts);

	ldraw_renderer::renderer_opengl_factory rof(&params_, mode_);
	mode_ = rof.get_rendering_mode();
	
	renderer_ = rof.create_renderer();
	renderer_->setup();
}

void render(int elapsed)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	
	glPushMatrix();
	glLoadIdentity();

	GLfloat degrees = elapsed / 3000.0 * 90.0;
	GLfloat position[] = {0.0f, -10000.0f, 0.0f, 1.0f};
	position[0] = std::sin(-degrees / 180.0 * M_PI) * 5000.0;
	position[2] = std::cos(-degrees / 180.0 * M_PI) * 5000.0;

	//glLightfv(GL_LIGHT1, GL_POSITION, position);
	
	glRotatef(degrees, 0.0, 1.0, 0.0);

	renderer_->render(model_->main_model());

	memsiz_ = ldraw_renderer::vbuffer_extension::get_total_memory_usage();

	glPopMatrix();

	glDisable(GL_LIGHTING);
	glColor3ub(255, 255, 255);
}

void resize(int width, int height)
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

