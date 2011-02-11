// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <cmath>
#include <iostream>
#include <stdexcept>

#include <libldr/metrics.h>
#include <libldr/model.h>

#include <renderer/parameters.h>
#include <renderer/mouse_rotation.h>

#ifndef KONSTRUKTOR_DB_UPDATER
#include "application.h"
#endif
#include "viewport.h"

#include "pixmaprenderer.h"

namespace Konstruktor
{

PixmapRenderer::PixmapRenderer(int width, int height, QGLWidget *shareWidget)
	: width_(width), height_(height), shareWidget_(shareWidget)
{
	QGLFormat fmt = QGLFormat::defaultFormat();
	fmt.setAlpha(true);
	fmt.setSampleBuffers(true);

	if (!QGLPixelBuffer::hasOpenGLPbuffers()) {
		throw std::runtime_error("OpenGL Pbuffer extension required.");
	}
	
	buffer_ = new QGLPixelBuffer(width_, height_, fmt, shareWidget_);
	buffer_->makeCurrent();	
	
	// Initialize GL
	ldraw_renderer::renderer_opengl_factory::rendering_mode rmode;
#ifndef KONSTRUKTOR_DB_UPDATER
	params_ = new ldraw_renderer::parameters(*Application::self()->renderer_params());
	rmode = ldraw_renderer::renderer_opengl_factory::mode_vbo;
#else
	params_ = new ldraw_renderer::parameters();
	params_->set_stud_rendering_mode(ldraw_renderer::parameters::stud_regular);
	params_->set_vbuffer_criteria(ldraw_renderer::parameters::vbuffer_everything);
	params_->set_shading(true);
	rmode = ldraw_renderer::renderer_opengl_factory::mode_varray;
#endif
	
	ldraw_renderer::renderer_opengl_factory ro(params_, rmode);
	renderer_ = ro.create_renderer();
	renderer_->set_base_color(ldraw::color(7));
	renderer_->setup();
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	
	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, width_, height_);
	
	buffer_->doneCurrent();
}

PixmapRenderer::~PixmapRenderer()
{
	delete renderer_;
	delete params_;
	
	delete buffer_;
}

void PixmapRenderer::setNewSize(int width, int height)
{
	width_ = width;
	height_ = height;
	
	delete buffer_;
	buffer_ = new QGLPixelBuffer(width_, height_, QGLFormat::defaultFormat(), shareWidget_);
	
	buffer_->makeCurrent();
	glViewport(0, 0, width_, height_);
	buffer_->doneCurrent();
}

QPixmap PixmapRenderer::renderToPixmap(ldraw::model *m, bool crop)
{
	buffer_->makeCurrent();
	
	//glDrawBuffer(GL_FRONT);
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	Viewport dvp;
	if (m) {
		// Setup viewport
		// TODO reuse code

		renderer_->setup();

		const ldraw::metrics *metric;
		ldraw::metrics metricp(const_cast<ldraw::model *>(m));
		metric = m->custom_data<ldraw::metrics>();
		if (!metric) {
			metricp.update();
			metric = &metricp;
		}
		const ldraw::vector &min = metric->min();
		const ldraw::vector &max = metric->max();
		
		Viewport vp;
		vp.left   = 1e30;
		vp.right  = -1e30;
		vp.top    = 1e30;
		vp.bottom = -1e30;
		
		ldraw::vector transformedVec[8];
		transformedVec[0] = ldraw_renderer::mouse_rotation::isometric_projection_matrix * min;
		transformedVec[1] = ldraw_renderer::mouse_rotation::isometric_projection_matrix * ldraw::vector(min.x(), min.y(), max.z());
		transformedVec[2] = ldraw_renderer::mouse_rotation::isometric_projection_matrix * ldraw::vector(min.x(), max.y(), min.z());
		transformedVec[3] = ldraw_renderer::mouse_rotation::isometric_projection_matrix * ldraw::vector(min.x(), max.y(), max.z());
		transformedVec[4] = ldraw_renderer::mouse_rotation::isometric_projection_matrix * ldraw::vector(max.x(), min.y(), min.z());
		transformedVec[5] = ldraw_renderer::mouse_rotation::isometric_projection_matrix * ldraw::vector(max.x(), min.y(), max.z());
		transformedVec[6] = ldraw_renderer::mouse_rotation::isometric_projection_matrix * ldraw::vector(max.x(), max.y(), min.z());
		transformedVec[7] = ldraw_renderer::mouse_rotation::isometric_projection_matrix * max;
		
		for (int i = 0; i < 8; i++) {
			if (transformedVec[i].x() < vp.left)
				vp.left = transformedVec[i].x();
			if (transformedVec[i].x() > vp.right)
				vp.right = transformedVec[i].x();
			if (transformedVec[i].y() > vp.bottom)
				vp.bottom = transformedVec[i].y();
			if (transformedVec[i].y() < vp.top)
				vp.top = transformedVec[i].y();
		}
		
		float fxlen = std::fabs(vp.right  - vp.left)*0.5f; 
		float fylen = std::fabs(vp.bottom - vp.top )*0.5f;
		
		vp.left   -= fxlen*0.1f;
		vp.right  += fxlen*0.1f;
		vp.top    -= fylen*0.1f;
		vp.bottom += fylen*0.1f;
		vp.aspectRatio = fxlen/fylen;
		dvp = vp;
		
		float median, d;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (std::fabs(vp.bottom-vp.top)*((float)width_/height_) >= std::fabs(vp.right-vp.left)) {
			median = (vp.right + vp.left) * 0.5f;
			d = std::fabs((vp.right-median)*((float)width_/height_))/vp.aspectRatio;
			vp.left = median - d;
			vp.right = median + d;
			glOrtho(vp.left, vp.right, vp.bottom, vp.top, 10000.0f, -10000.0f);
		} else {
			median = (vp.top + vp.bottom) * 0.5f;
			d = std::fabs((vp.bottom-median)/((float)width_/(float)height_))*vp.aspectRatio;
			vp.bottom = median + d;
			vp.top = median - d;
			glOrtho(vp.left, vp.right, vp.bottom, vp.top, 10000.0f, -10000.0f);
		}

		glMultMatrixf(ldraw_renderer::mouse_rotation::isometric_projection_matrix.transpose().get_pointer());
		
		// Draw to pixbuf
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		renderer_->render(m);
	} else {
		crop = false;
	}
	
	glFlush();
	
	buffer_->doneCurrent();
	
	if (crop) {
		int w, h;
		float xl = std::fabs(dvp.right-dvp.left);
		float yl = std::fabs(dvp.bottom-dvp.top);

		if (xl == 0.0f || yl == 0.0f) {
			QPixmap np = QPixmap(16, 16);
			np.fill(Qt::transparent);
			
			return np;
		}
		
		if (xl >= yl) {
			w = width_;
			h = (int)(width_ * (yl/xl));
		} else {
			h = height_;
			w = (int)(height_ * (xl/yl));
		}
		
		return QPixmap::fromImage(buffer_->toImage().copy(width_/2 - w/2, height_/2 - h/2, w, h));
	} else {
		QPixmap np = QPixmap(16, 16);
		np.fill(Qt::transparent);
		
		return np;
	}
}

}
