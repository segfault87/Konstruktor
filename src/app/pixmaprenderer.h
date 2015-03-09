// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _PIXMAPRENDERER_H_
#define _PIXMAPRENDERER_H_

#include <QGLFormat>

#include <renderer/renderer_opengl.h>

class QGLContext;
class QGLWidget;
class QThread;

namespace Konstruktor
{
class RendererPixelBuffer;

class PixmapRenderer
{
 public:
  PixmapRenderer(int width, int height, QGLWidget *shareWidget = 0L);
  ~PixmapRenderer();

  void setNewSize(int width, int height);
  
  QPixmap renderToPixmap(ldraw::model *m, bool crop = false);
  
 private:
  ldraw_renderer::renderer_opengl *renderer_;
  ldraw_renderer::parameters *params_;
  
  int width_;
  int height_;
  
  RendererPixelBuffer *buffer_;
  QGLWidget *shareWidget_;
};

}

#endif
