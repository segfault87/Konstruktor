// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _RENDERWIDGET_H_
#define _RENDERWIDGET_H_

#include <GL/gl.h>

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

class KonstruktorMainWindow;
class KonstruktorVisibilityExtension;

class KonstruktorRenderWidget : public QGLWidget
{
	Q_OBJECT;
	
  public:
	enum ViewportMode { Top, Bottom, Front, Back, Left, Right, Free, Uninitialized };
	enum Behavior { Idle, Placing, Moving, Dragging, Rotating, Panning };
	
	KonstruktorRenderWidget(KonstruktorMainWindow *mainwindow, KonstruktorDocument **document, ViewportMode viewport, QGLContext *context, QGLWidget *shareWidget, QWidget *parent = 0L);
	~KonstruktorRenderWidget();

	ViewportMode viewportMode() const;
	ldraw::vector viewportCoordinate(const QPoint &dim) const;
	QPoint globalCoordinate(const ldraw::vector &vec) const;
	int depthAxis() const;

  signals:
	void objectDropped(const QString &filename, const ldraw::matrix &matrix, const ldraw::color &color);
	void madeSelection(const std::list<int> &selection);
	void translateObject(const ldraw::vector &vector);
						 
  public slots:
	void modelChanged(ldraw::model *model);
	void selectionChanged(const QSet<int> &set);
	void setViewport(ViewportMode mode);
	void readConfig();
	
  private:
	void set3DViewport();
	void updatePositionVector(const QPoint &pos);
	void rotate();

	void renderPointArray();
	void renderGrid(float xg, float yg, int xc, int yc, float xo, float yo, float zo);

	void reapplyConfigurations();
	void initializeGridVbo();
	
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
	KonstruktorDocument **activeDocument_;
	ldraw::model *tmodel_;
	ldraw_renderer::renderer_opengl *renderer_;
	ldraw_renderer::parameters *params_;
	KonstruktorVisibilityExtension *tvset_;
	KonstruktorSelection *tsset_;

	float projectionMatrix_[16];
	ViewportMode viewportMode_;
	QString viewportName_;
	QSize textSize_;
	int screenId_;
	bool initialized_;
	
	int width_, height_;
	float length_;

	Behavior behavior_;
	QPoint lastPos_;
	QRect region_;
	ldraw::vector translation_;
	ldraw::metrics objectmetrics_;
	KonstruktorViewport stretched_;

	QAction *viewportActions_[VIEWPORT_TYPES];
	QActionGroup *viewportActionGroup_;

	GLuint gridVbo_[2];
	float gridResolution_;
	int gridRows_, gridColumns_;
	float gridX_, gridY_, gridZ_;
	ldraw_renderer::parameters::render_method renderMode_, dragMode_;
	QColor highlightColor_;

	KonstruktorMainWindow *parent_;
};

#endif
