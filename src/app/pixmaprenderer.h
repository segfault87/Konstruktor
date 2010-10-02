// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _PIXMAPRENDERER_H_
#define _PIXMAPRENDERER_H_

#include <QGLFormat>
#include <QGLPixelBuffer>

#include <renderer/renderer_opengl.h>

class QGLContext;

class KonstruktorPixmapRenderer : public renderer_opengl
{
  public:
	KonstruktorPixmapRenderer(int width, int height);
	~KonstruktorPixmapRenderer();
	
	void setNewSize(int width, int height);
	
	QPixmap render(ldraw::model *m, bool crop = false);
	
  private:
	int width_;
	int height_;
	
	QGLFormat format_;
	QGLPixelBuffer *buffer_;
};

#endif
