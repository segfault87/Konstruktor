// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _RENDERWIDGET_H_
#define _RENDERWIDGET_H_

#if defined(WIN32)
#include <windows.h>
#endif

#include <list>
#include <set>

#include <QGLWidget>
#include <QPixmap>
#include <QPoint>
#include <QSet>
#include <QSize>
#include <QString>

#include <libldr/metrics.h>
#include <libldr/model.h>
#include <renderer/parameters.h>
#include <renderer/renderer_opengl.h>

#include "document.h"
#include "selection.h"

#define MAGNIFICATION_FACTOR 1.375f
#define VIEWPORT_TYPES 7

class QAction;
class QActionGroup;
class QGLContext;

namespace Konstruktor
{

class MainWindow;
class VisibilityExtension;

void initializeGridVbo();

class RenderWidget : public QGLWidget
{
  Q_OBJECT;
  
 public:
  enum ViewportMode { Top, Bottom, Front, Back, Left, Right, Free, Uninitialized };
  enum Behavior { Idle, Placing, Moving, MovingByAxis,
                  Dragging, Rotating, RotatingByAxis, Panning };
  enum SelectionMethod { Normal, Addition, Subtraction, Intersection };
  enum AnchorMode { AxisNone, AxisX, AxisY, AxisZ };
  
  RenderWidget(MainWindow *mainwindow, Document **document, ViewportMode viewport,
               QGLContext *context, QGLWidget *shareWidget, QWidget *parent = 0L);
  ~RenderWidget();
  
  ViewportMode viewportMode() const;
  ldraw::vector viewportCoordinate(const QPoint &dim) const;
  QPoint globalCoordinate(const ldraw::vector &vec) const;
  int depthAxis() const;
  
  static ViewportMode getViewportMode(int idx);
  
 signals:
  void objectDropped(const QString &filename, const ldraw::matrix &matrix,
                     const ldraw::color &color);
  void madeSelection(const std::list<int> &selection,
                     RenderWidget::SelectionMethod method = Normal);
  void translateObject(const ldraw::matrix &matrix);
                                                   
 public slots:
  void modelChanged(ldraw::model *model);
  void selectionChanged(const QSet<int> &set);
  void setViewport(ViewportMode mode);
  void readConfig();
  void anchorChanged();
  
 private:
  void set3DViewport();
  void updatePositionVector(const QPoint &pos);
  void rotate();
  ldraw::matrix retranslate(const ldraw::matrix &original) const;
  ldraw::vector unproject(const QPoint &position, const ldraw::matrix &modelView);
  
  AnchorMode anchorHitTest(int x, int y);
  void renderPointArray() const;
  void renderGrid(float xg, float yg, int xc, int yc, float xo, float yo, float zo) const;
  void renderAnchor() const;
  
  void reapplyConfigurations();
  
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);
  void paintEvent(QPaintEvent *event);
  
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);
  
  void dragEnterEvent(QDragEnterEvent *event);
  void dragLeaveEvent(QDragLeaveEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);
  void dropEvent(QDropEvent *event);
  
 private:
  Document **activeDocument_;
  ldraw::model *currentModel_;
  
  ldraw_renderer::renderer_opengl *renderer_;
  ldraw_renderer::parameters *params_;
  VisibilityExtension *tvset_;
  Selection *tsset_;
  IntermediateSelection *tisset_;
  
  float projectionMatrix_[16];
  ViewportMode viewportMode_;
  QString viewportName_;
  QSize textSize_;
  int screenId_;
  bool initialized_;
  
  int width_, height_;
  float length_;
  
  Behavior behavior_;
  ldraw::matrix anchor_;
  bool anchorEnabled_;
  AnchorMode anchorMode_, anchorHover_;
  QPoint lastPos_;
  QRect region_;
  bool isRegion_;
  SelectionMethod selectionMethod_;
  ldraw::matrix translation_;
  ldraw::metrics objectmetrics_;
  ldraw::matrix objectmatrix_;
  ldraw::color objectcolor_;
  ldraw::vector anchorstart_;
  Viewport stretched_;
  
  QAction *viewportActions_[VIEWPORT_TYPES];
  QActionGroup *viewportActionGroup_;
  
  GLuint gridVbo_[2];
  float gridResolution_;
  int gridRows_, gridColumns_;
  float gridX_, gridY_, gridZ_;
  ldraw_renderer::parameters::render_method renderMode_, dragMode_;
  QColor highlightColor_;
  QColor highlightDragColor_;
  
  MainWindow *parent_;
};

}

#endif
