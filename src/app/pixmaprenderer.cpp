// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <cmath>
#include <iostream>
#include <stdexcept>

#include <libldr/metrics.h>
#include <libldr/model.h>

#include <renderer/parameters.h>
#include <renderer/mouse_rotation.h>

#include <QGLPixelBuffer>
#include <QtDebug>

#ifndef KONSTRUKTOR_DB_UPDATER
#include "application.h"
#endif
#include "viewport.h"

#include "pixmaprenderer.h"


namespace Konstruktor
{

class RendererPixelBuffer
{
 public:
  RendererPixelBuffer ( int width, int height, const QGLFormat & format = QGLFormat::defaultFormat(), QGLWidget * shareWidget_ = 0 ) : buffer(0), context(0)
  {
    if (QGLPixelBuffer::hasOpenGLPbuffers()) {
      buffer = new QGLPixelBuffer(width, height, format, shareWidget_);
      if (buffer->isValid())
        return;
      else {
        qDebug() << "Pixelbuffer instantiation failed - uses non accelerated implementation";
        delete buffer;
        buffer = 0L;
      }
    }
    else 
      qDebug() << "no Pixelbuffer support - uses non accelerated implementation";
    
    context = new QGLContext(format);
    if (!context->create()) {
      qDebug() << "could not create QGLContext - there may no display" ;
      
      delete context;
      context = 0L;
    }
  }
  
  ~RendererPixelBuffer() {
    if (buffer)
      delete buffer;
    
    if (context)
      delete context;
  }
  
  bool isValid() {
    return buffer || context;
  }
  
  bool makeCurrent() {
    if (buffer) {
      return buffer->makeCurrent();
    } else if (context) {
      context->makeCurrent();
      return true;
    }
    else {
      return false;
    }
  }
  
  bool doneCurrent() {
    if (buffer) {
      return buffer->doneCurrent();
    } else if (context) {
      context->doneCurrent();
      return true;
    } else {
      return false;
    }
  }
  
  QPixmap toPixmap(int x, int y, int width, int height) {
    if (buffer) {
      return QPixmap::fromImage(buffer->toImage().copy(x, y, width, height));
    } else if (context) {
      context->makeCurrent();
      QImage img = qt_gl_read_framebuffer(QSize(width,height), context->format().alpha(), true);
      glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
      context->doneCurrent();
      convertFromGLImage(img, width, height, context->format().alpha(), true);
      return QPixmap::fromImage(img);
		} else {
      printf("null\n");
      return QPixmap();
    }
  }
  
 private: 
  QGLPixelBuffer *buffer;
  QGLContext *context;
  
  static void convertFromGLImage(QImage &img, int w, int h, bool alpha_format, bool include_alpha) {
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
      // OpenGL gives RGBA; Qt wants ARGB
      uint *p = (uint*)img.bits();
      uint *end = p + w*h;
      if (alpha_format && include_alpha) {
        while (p < end) {
          uint a = *p << 24;
          *p = (*p >> 8) | a;
          p++;
        }
      } else {
        // This is an old legacy fix for PowerPC based Macs, which
        // we shouldn't remove
        while (p < end) {
          *p = 0xff000000 | (*p>>8);
          ++p;
        }
      }
    } else {
      // OpenGL gives ABGR (i.e. RGBA backwards); Qt wants ARGB
      for (int y = 0; y < h; y++) {
        uint *q = (uint*)img.scanLine(y);
        for (int x=0; x < w; ++x) {
          const uint pixel = *q;
          if (alpha_format && include_alpha) {
            *q = ((pixel << 16) & 0xff0000) | ((pixel >> 16) & 0xff)
                | (pixel & 0xff00ff00);
          } else {
            *q = 0xff000000 | ((pixel << 16) & 0xff0000)
                | ((pixel >> 16) & 0xff) | (pixel & 0x00ff00);
          }
          
          q++;
        }
      }
      
    }
    img = img.mirrored();
  }
  
  QImage qt_gl_read_framebuffer(const QSize &size, bool alpha_format, bool include_alpha) {
    QImage img(size, (alpha_format && include_alpha) ? QImage::Format_ARGB32
               : QImage::Format_RGB32);
    int w = size.width();
    int h = size.height();
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
    convertFromGLImage(img, w, h, alpha_format, include_alpha);
    return img;
  }
};

PixmapRenderer::PixmapRenderer(int width, int height, QGLWidget *shareWidget)
	: width_(width), height_(height), shareWidget_(shareWidget)
{
  QGLFormat fmt = QGLFormat::defaultFormat();
  fmt.setAlpha(true);
  fmt.setSampleBuffers(true);
  
  buffer_ = new RendererPixelBuffer(width_, height_, fmt, shareWidget_);
  if (!buffer_->isValid()) {
    /* first fallback */
    delete buffer_;
    
    QGLFormat fmt = QGLFormat::defaultFormat();
    fmt.setAlpha(true);
    buffer_ = new RendererPixelBuffer(width_, height_, fmt, shareWidget_);
    
    if (!buffer_->isValid()) {
      /* second fallback */
      delete buffer_;
      
      buffer_ = new RendererPixelBuffer(width_, height_, QGLFormat::defaultFormat(), shareWidget_);
    }
  }
  
  buffer_->makeCurrent();	
  
  // Initialize GL
  ldraw_renderer::renderer_opengl_factory::rendering_mode rmode;
#ifndef KONSTRUKTOR_DB_UPDATER
  switch (Application::self()->config()->renderingMode()) {
    case Config::VBO:
    default:
      rmode = ldraw_renderer::renderer_opengl_factory::mode_vbo;
      break;
    case Config::VertexArray:
      rmode = ldraw_renderer::renderer_opengl_factory::mode_varray;
      break;
    case Config::Immediate:
      rmode = ldraw_renderer::renderer_opengl_factory::mode_immediate;
      break;
  }
  
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
  buffer_ = new RendererPixelBuffer(width_, height_, QGLFormat::defaultFormat(), shareWidget_);
  
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
  dvp.left   = 1e30;
  dvp.right  = -1e30;
  dvp.top    = 1e30;
  dvp.bottom = -1e30;
  
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
    return buffer_->toPixmap(width_/2 - w/2, height_/2 - h/2, w, h);
  } else {
    QPixmap np = QPixmap(16, 16);
    np.fill(Qt::transparent);
    
    return np;
  }
}

}
