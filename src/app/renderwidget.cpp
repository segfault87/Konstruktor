// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <limits.h>

#include <cmath>

#include <libldr/elements.h>
#include <libldr/metrics.h>
#include <libldr/utils.h>

#include <QAction>
#include <QActionGroup>
#include <QFontMetrics>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>

#include "renderer/opengl_extension_vbo.h"

#include "document.h"
#include "editor.h"
#include "application.h"
#include "mainwindow.h"
#include "refobject.h"
#include "visibilityextension.h"

#include "renderwidget.h"

/* include later in order to avoid some header conflicts */
#include <renderer/opengl_extension_vbo.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

namespace Konstruktor
{

#define EXIT_IF_NO_DOCUMENT if (!(*activeDocument_)) return

static const QString viewportNames[VIEWPORT_TYPES] = {
  QObject::tr("Top"),
  QObject::tr("Bottom"),
  QObject::tr("Front"),
  QObject::tr("Back"),
  QObject::tr("Left"),
  QObject::tr("Right"),
  QObject::tr("3D")
};


void initializeGridVbo()
{
  ldraw_renderer::opengl_extension_vbo *vbo = ldraw_renderer::opengl_extension_vbo::self();
  
  
}

RenderWidget::RenderWidget(MainWindow *mainwindow, Document **document,
                           ViewportMode viewport, QGLContext *context,
                           QGLWidget *shareWidget, QWidget *parent)
    : QGLWidget(context, parent, shareWidget), objectmetrics_(0L)
{
  viewportMode_ = Uninitialized;
  parent_ = mainwindow;
  
  currentModel_ = 0L;
  tvset_ = 0L;
  
  tsset_ = new Selection();
  tisset_ = new IntermediateSelection(tsset_);
  
  activeDocument_ = document;
  behavior_ = Idle;
  initialized_ = false;

  anchorEnabled_ = false;
  anchorMode_ = AxisNone;
  anchorHover_ = AxisNone;
  
  readConfig();
  gridVbo_[0] = 0;
  gridVbo_[1] = 0;
  
  makeCurrent();
  
  ldraw_renderer::renderer_opengl_factory::rendering_mode rm;
  switch (Application::self()->config()->renderingMode()) {
    case Config::VBO:
    default:
      rm = ldraw_renderer::renderer_opengl_factory::mode_vbo;
      break;
    case Config::VertexArray:
      rm = ldraw_renderer::renderer_opengl_factory::mode_varray;
      break;
    case Config::Immediate:
      rm = ldraw_renderer::renderer_opengl_factory::mode_immediate;
      break;
  }
  
  params_ = new ldraw_renderer::parameters(*Application::self()->renderer_params());
  renderer_ = ldraw_renderer::renderer_opengl_factory(params_, rm).create_renderer();
  
  reapplyConfigurations();
  
  params_->set_rendering_mode(renderMode_);
  
  /* init actions */
  viewportActionGroup_ = new QActionGroup(this);
  for (int i = 0; i < VIEWPORT_TYPES; ++i) {
    viewportActions_[i] = new QAction(viewportNames[i], this);
    viewportActionGroup_->addAction(viewportActions_[i]);
    viewportActions_[i]->setCheckable(true);
    viewportActions_[i]->setData(i);
  }
  
  setViewport(viewport);
  
  doneCurrent();
  
  setAcceptDrops(true);
  setMouseTracking(true);
}

RenderWidget::~RenderWidget()
{
  delete renderer_;
  delete params_;
}

RenderWidget::ViewportMode RenderWidget::viewportMode() const
{
  return viewportMode_;
}

ldraw::vector RenderWidget::viewportCoordinate(const QPoint &dim) const
{
  float xr = stretched_.left +
      ((stretched_.right  - stretched_.left) / width())  * dim.x();
  float yr = stretched_.top +
      ((stretched_.bottom - stretched_.top) / height()) * dim.y();
  
  switch (viewportMode_) {
    case Front:
      return ldraw::vector(xr, yr, 0.0f);
    case Back:
      return ldraw::vector(-xr, yr, 0.0f);
    case Top:
      return ldraw::vector(xr, 0.0f, -yr);
    case Bottom:
      return ldraw::vector(-xr, 0.0f, yr);
    case Left:
      return ldraw::vector(0.0f, yr, -xr);
    case Right:
      return ldraw::vector(0.0f, yr, xr);
    default:
      return ldraw::vector(0.0f, 0.0f, 0.0f);
  }
}

QPoint RenderWidget::globalCoordinate(const ldraw::vector &vec) const
{
  float xc, yc;
  
  switch (viewportMode_) {
    case Top:
      xc = vec.x();
      yc = -vec.z();
      break;
    case Bottom:
      xc = -vec.x();
      yc = vec.z();
      break;
    case Left:
      xc = -vec.z();
      yc = vec.y();
      break;
    case Right:
      xc = vec.z();
      yc = vec.y();
      break;
    case Front:
      xc = vec.x();
      yc = vec.y();
      break;
    case Back:
      xc = -vec.x();
      yc = vec.y();
      break;
    default:
      xc = 0.0f;
      yc = 0.0f;
  }
  
  return QPoint(
      (int)((xc - stretched_.left) / (stretched_.right  - stretched_.left) * width()),
      (int)((yc - stretched_.top)  / (stretched_.bottom - stretched_.top)  * height())
                );
}

int RenderWidget::depthAxis() const
{
  switch (viewportMode_) {
    case Top:
    case Bottom:
      return 1; // y-axis
    case Front:
    case Back:
      return 2; // z-axis
    case Left:
    case Right:
      return 0; // x-axis
    default:
      return -1;
  }
}

RenderWidget::ViewportMode RenderWidget::getViewportMode(int idx)
{
  switch (idx) {
    case 0:
      return Top;
    case 1:
      return Bottom;
    case 2:
      return Front;
    case 3:
      return Back;
    case 4:
      return Left;
    case 5:
      return Right;
    case 6:
    default:
      return Free;
  }
}

void RenderWidget::modelChanged(ldraw::model *)
{
  tsset_->resetSelection();
  anchorChanged();
}

void RenderWidget::selectionChanged(const QSet<int> &set)
{
  tsset_->setSelection(set);
  anchorChanged();
  
  update();
}

void RenderWidget::setViewport(ViewportMode mode)
{
  if(viewportMode_ != mode) {
    viewportMode_ = mode;
    viewportName_ = viewportNames[mode];
    
    QFont defaultfont;
    textSize_ = QFontMetrics(defaultfont).size(Qt::TextSingleLine, viewportName_);
    
    viewportActions_[mode]->setChecked(true);
    
    update();
  }
}

void RenderWidget::readConfig()
{
  Config *conf = Application::self()->config();
  
  gridResolution_ = conf->gridResolution();
  gridRows_ = conf->gridRows();
  gridColumns_ = conf->gridColumns();
  gridX_ = conf->gridXOffset();
  gridY_ = conf->gridYOffset();
  gridZ_ = conf->gridZOffset();
  
  highlightColor_ = conf->highlightColor();
  highlightDragColor_ = conf->highlightDragColor();
  
  switch (conf->dragMode()) {
    case Config::Full:
      dragMode_ = ldraw_renderer::parameters::model_full;
      break;
    case Config::Edges:
      dragMode_ = ldraw_renderer::parameters::model_edges;
      break;
    case Config::BoundingBoxes:
      dragMode_ = ldraw_renderer::parameters::model_boundingboxes;
  }
  
  switch (conf->renderMode()) {
    case Config::Full:
      renderMode_ = ldraw_renderer::parameters::model_full;
      break;
    case Config::Edges:
      renderMode_ = ldraw_renderer::parameters::model_edges;
      break;
    case Config::BoundingBoxes:
      renderMode_ = ldraw_renderer::parameters::model_boundingboxes;
  }
  
  reapplyConfigurations();
}

void RenderWidget::anchorChanged()
{
  const ldraw::element_ref *uniqueSelection = tsset_->getUniqueRef();
  Editor::RotationPivot pivotMode = Editor::instance()->rotationPivotMode();

  anchorEnabled_ = false;
  
  if (uniqueSelection) {
    anchorEnabled_ = true;
    if (pivotMode == Editor::PivotEach) {
      anchor_ = uniqueSelection->get_matrix();
    } else {
      ldraw::vector pivotvec = Editor::instance()->getPivot();
      anchor_ = ldraw::matrix();
      anchor_.set_translation_vector(pivotvec);
    }
  } else if (tsset_->hasSelection()) {
    if (pivotMode != Editor::PivotEach) {
      anchorEnabled_ = true;
      ldraw::vector pivotvec = Editor::instance()->getPivot();
      anchor_ = ldraw::matrix();
      anchor_.set_translation_vector(pivotvec);
    }
  }
}

void RenderWidget::set3DViewport()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  const Viewport *viewport;
  
  if (activeDocument_ && *activeDocument_)
    viewport = &((*activeDocument_)->getViewport((int)viewportMode_));
  else
    viewport = &defaultViewport;
  
  float median, d;
  
  if (std::fabs(viewport->bottom-viewport->top) * ((float)width_/height_) >=
      std::fabs(viewport->right-viewport->left)) {
    median = (viewport->right + viewport->left) * 0.5f;
    d = std::fabs((viewport->right-median) * ((float)width_/height_))
        / viewport->aspectRatio;
    stretched_.left = median - d;
    stretched_.right = median + d;
    stretched_.bottom = viewport->bottom;
    stretched_.top = viewport->top;
    glOrtho(stretched_.left, stretched_.right,
            stretched_.bottom, stretched_.top,
            10000.0f, -10000.0f);
  } else {
    median = (viewport->top + viewport->bottom) * 0.5f;
    d = std::fabs((viewport->bottom-median) / ((float)width_/height_))
        * viewport->aspectRatio;
    stretched_.left = viewport->left;
    stretched_.right = viewport->right;
    stretched_.bottom = median + d;
    stretched_.top = median - d;
    glOrtho(stretched_.left, stretched_.right,
            stretched_.bottom, stretched_.top,
            10000.0f, -10000.0f);
  }

  rotate();
  
  glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix_);
}

// Map current cursor position to world coordinate
void RenderWidget::updatePositionVector(const QPoint &pos)
{
  const Editor *editor = Editor::instance();
  
  float dx = pos.x() / (float)width_ * std::fabs(stretched_.right - stretched_.left);
  float dy = pos.y() / (float)height_ * std::fabs(stretched_.bottom - stretched_.top);
  
  ldraw::vector v;

  switch (viewportMode_) {
    case Top:
      v = ldraw::vector(editor->snap(dx), 0.0f, -editor->snap(dy));
      break;
    case Bottom:
      v = ldraw::vector(editor->snap(dx), 0.0f, editor->snap(dy));
      break;
    case Front:
      v = ldraw::vector(editor->snap(dx), editor->snapYAxis(dy), 0.0f);
      break;
    case Back:
      v = ldraw::vector(-editor->snap(dx), editor->snapYAxis(dy), 0.0f);
      break;
    case Left:
      v = ldraw::vector(0.0f, editor->snapYAxis(dy), -editor->snap(dx));
      break;
    case Right:
      v = ldraw::vector(0.0f, editor->snapYAxis(dy), editor->snap(dx));
      break;
    default:
      break;
  }

  translation_ = ldraw::matrix();
  translation_.set_translation_vector(v);
}

// Set viewport
void RenderWidget::rotate()
{
  const ldraw::metrics *metrics;
  ldraw::vector vec;
  
  switch (viewportMode_) {
    case Free:
      metrics = (*activeDocument_)->getActiveModel()->custom_data<ldraw::metrics>();
      vec = (metrics->min_() + metrics->max_()) * 0.5f;
      glMultMatrixf((*activeDocument_)->getMouseRotation().get_matrix().transpose().get_pointer());
      glTranslatef(-vec.x(), -vec.y(), -vec.z());
      break;
    case Top:
      glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
      break;
    case Bottom:
      glRotatef(90.0f, -1.0f, 0.0f, 0.0f);
      break;
    case Back:
      glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
      break;
    case Left:
      glRotatef(90.0f, 0.0f, -1.0f, 0.0f);
      break;
    case Right:
      glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
      break;
    default:
      break;
  }
}

ldraw::matrix RenderWidget::retranslate(const ldraw::matrix &original) const
{
  ldraw::matrix o = original;
  ldraw::vector v = translation_.get_translation_vector();
  ldraw::vector tvec = o.get_translation_vector();

  switch (viewportMode_) {
    case Top:
    case Bottom:
      v.y() = tvec.y();
      break;
    case Front:
    case Back:
      v.z() = tvec.z();
      break;
    case Left:
    case Right:
      v.x() = tvec.x();
      break;
    default:
      ;
  }

  o.set_translation_vector(v);

  return o;
}

ldraw::vector RenderWidget::unproject(const QPoint &position)
{
  makeCurrent();

  GLdouble projectionCoerced[16];
  GLdouble modelviewCoerced[16];
  GLint viewport[4];
  GLdouble x, y, z;
  GLfloat depth;
  ldraw::matrix identity;
  
  for (int i = 0; i < 16; ++i) {
    projectionCoerced[i] = projectionMatrix_[i];
    modelviewCoerced[i] = identity.value(i / 4, i % 4);
  }
  
  glGetIntegerv(GL_VIEWPORT, viewport);

  glReadPixels(position.x(), position.y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

  gluUnProject(position.x(), position.y(), (GLdouble) depth,
               modelviewCoerced, projectionCoerced, viewport,
               &x, &y, &z);

  doneCurrent();

  return ldraw::vector(x, y, z);
}

RenderWidget::AnchorMode RenderWidget::anchorHitTest(int x, int y)
{
  GLint hits, viewport[4];
  GLuint selectionBuffer[20];

  glPushAttrib(GL_LINE_BIT);
  glLineWidth(7.0f);

  glSelectBuffer(5, selectionBuffer);
  glRenderMode(GL_SELECT);

  glGetIntegerv(GL_VIEWPORT, viewport);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluPickMatrix(x - 3, viewport[3] - (y + 3), 6, 6, viewport);
  glMultMatrixf(projectionMatrix_);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMultMatrixf((anchor_ * 3.0f).transpose().get_pointer());
  
  glInitNames();

  glPushName(0);
  glBegin(GL_LINES);
  glVertex3f(-10.0f, 0.0f, 0.0f);
  glVertex3f(10.0f, 0.0f, 0.0f);
  glEnd();
  glPopName();
  
  glPushName(1);
  glBegin(GL_LINES);
  glVertex3f(0.0f, -10.0f, 0.0f);
  glVertex3f(0.0f, 10.0f, 0.0f);
  glEnd();
  glPopName();

  glPushName(2);
  glBegin(GL_LINES);
  glVertex3f(0.0f, 0.0f, -10.0f);
  glVertex3f(0.0f, 0.0f, 10.0f);
  glEnd();
  glPopName();

  hits = glRenderMode(GL_RENDER);

  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glPopAttrib();

  if (hits < 0)
    return AxisNone;

  for (int i = 0; i < hits; ++i) {
    GLuint idx = selectionBuffer[i * 4 + 3];

    if (idx == 0)
      return AxisX;
    else if (idx == 1)
      return AxisY;
    else if (idx == 2)
      return AxisZ;
  }

  return AxisNone;
}

void RenderWidget::renderPointArray() const
{
  if (!currentModel_)
    return;
  
  const unsigned char c1[] = {255, 255, 255};
  const unsigned char c2[] = {0, 0, 0};
  const unsigned char *colors[] = {c1, c2};
  const float psizes[] = {8.0f, 6.0f};
  
  for (int i = 0; i < 2; ++i) {
    glColor3ubv(colors[i]);
    glPointSize(psizes[i]);
    
    glBegin(GL_POINTS);
    
    int j = 0;
    for (ldraw::model::const_iterator it = currentModel_->elements().begin();
         it != currentModel_->elements().end();
         ++it) {
      if ((*it)->get_type() == ldraw::type_ref) {
        ldraw::element_ref *r = CAST_AS_REF(*it);
        ldraw::model *rm = r->get_model();
	
        if (rm) {
          if (!tvset_->query(rm, j, 0)) {
            if (!rm->custom_data<ldraw::metrics>())
              rm->init_custom_data<ldraw::metrics>();
            
            const ldraw::metrics *rmm = rm->custom_data<ldraw::metrics>();
            const ldraw::matrix &rmt = r->get_matrix();
            ldraw::vector center = (rmt * rmm->min_() + rmt * rmm->max_()) * 0.5f;
            
            glVertex3fv(center.get_pointer());
          }				
        }
      }
      ++j;
    }
    
    glEnd();
  }
}

void RenderWidget::renderGrid(float xg, float yg,
                              int xc, int yc,
                              float xo, float yo, float zo) const
{
  glLineWidth(1.0f);
  
  switch (viewportMode_) {
    case Front:
    case Back:
      glColor4ub(32, 32, 32, 128);
      glEnable(GL_DEPTH_TEST);
      glBegin(GL_LINES);
      glVertex3f(-(xg*(float)xc)+xo, zo, 0.0f);
      glVertex3f( (xg*(float)xc)+xo, zo, 0.0f);
      glEnd();
      glDisable(GL_DEPTH_TEST);
      return;
      
    case Left:
    case Right:
      glColor4ub(32, 32, 32, 128);
      glEnable(GL_DEPTH_TEST);
      glBegin(GL_LINES);
      glVertex3f(0.0f, zo, -(yg*(float)yc)+yo);
      glVertex3f(0.0f, zo,  (yg*(float)yc)+yo);
      glEnd();
      glDisable(GL_DEPTH_TEST);
      return;
      
    default:
      glBegin(GL_LINES);
      
      glColor4ub(32, 32, 32, 128);
      
      glVertex3f(-(xg*(float)xc)+xo, zo, yo);
      glVertex3f( (xg*(float)xc)+xo, zo, yo);
      
      glVertex3f(xo, zo, -(yg*(float)yc)+yo);
      glVertex3f(xo, zo,  (yg*(float)yc)+yo);
      
      glColor4ub(128, 128, 128, 96);
      
      for (int i = 1; i <= xc; i++) {
        glVertex3f(-(xg*(float)i)+xo, zo, -(yg*(float)yc)+yo);
        glVertex3f(-(xg*(float)i)+xo, zo,  (yg*(float)yc)+yo);
	
        glVertex3f( (xg*(float)i)+xo, zo, -(yg*(float)yc)+yo);
        glVertex3f( (xg*(float)i)+xo, zo,  (yg*(float)yc)+yo);
      }
      
      for (int i = 1; i <= yc; i++) {
        glVertex3f(-(xg*(float)xc)+xo, zo, -(yg*(float)i)+yo);
        glVertex3f( (xg*(float)xc)+xo, zo, -(yg*(float)i)+yo);
	
        glVertex3f(-(xg*(float)xc)+xo, zo,  (yg*(float)i)+yo);
        glVertex3f( (xg*(float)xc)+xo, zo,  (yg*(float)i)+yo);
      }
      
      glEnd();
      
      return;
  }
}

void RenderWidget::renderAnchor() const
{
  glDisable(GL_DEPTH_TEST);

  ldraw::vector a = anchor_.get_translation_vector();
  ldraw::vector b = translation_.get_translation_vector();

  glPushMatrix();
  glMultMatrixf((translation_ * anchor_ * 3.0f).transpose().get_pointer());
  
  glLineWidth(3.0f);
  glBegin(GL_LINES);

  if (anchorHover_ == AxisX)
    glColor3f(1.0f, 1.0f, 1.0f);
  else
    glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3f(-10.0f, 0.0f, 0.0f);
  glVertex3f(-1.0f, 0.0f, 0.0f);
  glVertex3f(1.0f, 0.0f, 0.0f);
  glVertex3f(10.0f, 0.0f, 0.0f);

  if (anchorHover_ == AxisY)
    glColor3f(1.0f, 1.0f, 1.0f);
  else
    glColor3f(0.0f, 1.0f, 0.0f);
  glVertex3f(0.0f, -10.0f, 0.0f);
  glVertex3f(0.0f, -1.0f, 0.0f);
  glVertex3f(0.0f, 1.0f, 0.0f);
  glVertex3f(0.0f, 10.0f, 0.0f);

  if (anchorHover_ == AxisZ)
    glColor3f(1.0f, 1.0f, 1.0f);
  else
    glColor3f(0.0f, 0.0f, 1.0f);
  glVertex3f(0.0f, 0.0f, -10.0f);
  glVertex3f(0.0f, 0.0f, -1.0f);
  glVertex3f(0.0f, 0.0f, 1.0f);
  glVertex3f(0.0f, 0.0f, 10.0f);

  glEnd();
  
  glPopMatrix();
  
  glEnable(GL_DEPTH_TEST);
}

void RenderWidget::reapplyConfigurations()
{
  initializeGridVbo();
}

void RenderWidget::initializeGL()
{
  // libldr_renderer initialize proc
  makeCurrent();
  renderer_->setup();
  
  glDepthFunc(GL_LEQUAL);
  
  // Set default color and depth
  qglClearColor(Application::self()->config()->backgroundColor());
	glClearDepth(1.0f);
        
	doneCurrent();
}

void RenderWidget::paintGL()
{
  makeCurrent();
  
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  
  /* disable shading if not free view */
  if (viewportMode_ != Free)
    params_->set_shading(false);
  else
    params_->set_shading(true);
  
  if(*activeDocument_) {
    ldraw::model *curmodel = (*activeDocument_)->getActiveModel();
    if (curmodel != currentModel_) {
      currentModel_ = curmodel;
      tvset_ = VisibilityExtension::query(curmodel);
      tsset_->setModel(currentModel_);
    }
    
    if (Application::self()->config()->multisampling())
      glEnable(GL_MULTISAMPLE);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    set3DViewport();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_BLEND);
    
    if (params_->get_rendering_mode() == ldraw_renderer::parameters::model_boundingboxes)
      glColor3ub(0, 0, 0);
    renderer_->render(curmodel, tvset_);
    
    glDisable(GL_LIGHTING);
    
    if (behavior_ == Placing) {
      glDisable(GL_DEPTH_TEST);
      qglColor(highlightColor_);
      glLineWidth(2.0f);
      glPushMatrix();
      
      glMultMatrixf((translation_ * objectmatrix_).transpose().get_pointer());
      
      renderer_->render_bounding_box(objectmetrics_);
      
      glPopMatrix();
    } else if (behavior_ == Dragging) {
      if (isRegion_) {
        glDisable(GL_DEPTH_TEST);
        renderPointArray();
      }
    }
    
    if (tsset_->hasSelection()) {
      qglColor(highlightColor_);
      glLineWidth(2.0f);
      glPushMatrix();
      
      if (behavior_ == Moving || behavior_ == MovingByAxis)
        glMultMatrixf(translation_.transpose().get_pointer());
      
      params_->set_rendering_mode(ldraw_renderer::parameters::model_boundingboxes);
      tsset_->setInverted(true);
      renderer_->render(currentModel_, tsset_);
      tsset_->setInverted(false);
      
      if (behavior_ == Idle)
        params_->set_rendering_mode(renderMode_);
      else
        params_->set_rendering_mode(dragMode_);
      
      glPopMatrix();
    }
    
    if (tisset_->hasSelection()) {
      qglColor(highlightDragColor_);
      glLineWidth(2.0f);
      glPushMatrix();
      
      if (behavior_ == Moving || behavior_ == MovingByAxis)
        glMultMatrixf(translation_.transpose().get_pointer());
      
      params_->set_rendering_mode(ldraw_renderer::parameters::model_boundingboxes);
      tisset_->setSelectionMethod(selectionMethod_);
      renderer_->render(currentModel_, tisset_);
      
      if (behavior_ == Idle)
        params_->set_rendering_mode(renderMode_);
      else
        params_->set_rendering_mode(dragMode_);
      
      glPopMatrix();
    }

    if (anchorEnabled_)
      renderAnchor();
    
    if (Application::self()->config()->drawGrids())
      renderGrid(gridResolution_,
                 gridResolution_,
                 gridRows_,
                 gridColumns_,
                 gridX_,
                 gridY_,
                 gridZ_);
  }
  
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  
  glPopAttrib();
}

void RenderWidget::resizeGL(int width, int height)
{
  glViewport(0, 0, width, height);
  width_ = width;
  height_ = height;
  length_ = std::sqrt(pow((double)width_, 2.0) + pow((double)height_, 2.0));
}

void RenderWidget::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  
  QPainter p;
  p.begin(this);
  
  makeCurrent();
  
  p.setRenderHint(QPainter::Antialiasing);

  initializeGL();
  if (!initialized_) {
    resizeGL(width(), height());
    
    initialized_ = true;
  }
  
  paintGL();
  
  glDisable(GL_DEPTH_TEST);
  
  // Viewport name
  if (behavior_ == Dragging) {
    p.setBrush(QBrush(QColor(51, 115, 186, 96)));
    p.setPen(QPen(QColor(25, 59, 93)));
    p.drawRect(region_);
  }
  
  p.setBrush(QBrush(QColor(24, 24, 24, 160)));
  p.setPen(Qt::NoPen);
  p.drawRect(20, 20, textSize_.width()+10, textSize_.height()+10);
  p.setPen(Qt::white);
  p.drawText(25, 25, textSize_.width(), textSize_.height(),
             Qt::AlignVCenter, viewportName_);
  
  p.end();
  
  doneCurrent();
}

void RenderWidget::mousePressEvent(QMouseEvent *event)
{
  // Cancel select
  if (behavior_ != Idle && event->button() & Qt::RightButton) {
    tisset_->clear();
    
    behavior_ = Idle;
    params_->set_rendering_mode(renderMode_);
    
    update();
  }
  
  QRect rect(20, 20, textSize_.width()+10, textSize_.height()+10);
  if (event->button() & Qt::LeftButton && rect.contains(event->pos())) {
    QMenu viewportMenu(this);
    unsigned int activeViewports = parent_->viewportModes();
    for (int i = 0; i < VIEWPORT_TYPES; ++i) {
      viewportMenu.addAction(viewportActions_[i]);
      if (activeViewports & (1 << i) && !viewportActions_[i]->isChecked())
        viewportActions_[i]->setEnabled(false);
      else
        viewportActions_[i]->setEnabled(true);
    }
    
    QAction *result = viewportMenu.exec(event->globalPos());
    if (!result)
      return;
    
    setViewport((ViewportMode)result->data().toInt());
    
    return;
  }
  
  if (anchorHover_ != AxisNone && event->button() & Qt::LeftButton) {
    anchorMode_ = anchorHover_;
    anchorstart_ = unproject(event->pos());
    behavior_ = MovingByAxis;

    update();
  } else if (event->button() & Qt::MidButton) {
    params_->set_rendering_mode(dragMode_);
    behavior_ = Panning;
    lastPos_ = event->pos();
    
    update();
  } else if (viewportMode_ == Free && event->button() & Qt::RightButton) {
    params_->set_rendering_mode(dragMode_);
    behavior_ = Rotating;
    
    (*activeDocument_)->getMouseRotation().press_event(event->pos().x(),
                                                       event->pos().y(),
                                                       width_, height_);
    
    update();
  } else if (event->button() & Qt::LeftButton) {
    makeCurrent();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    if (viewportMode_ != Free && tsset_->hasSelection() &&
        tsset_->getSelection()->size() > 0 &&
        renderer_->hit_test(projectionMatrix_,
                            const_cast<float *>(ldraw::matrix().get_pointer()),
                            event->pos().x(),
                            event->pos().y(),
                            1,
                            1,
                            (*activeDocument_)->getActiveModel(),
                            tsset_)
        ) {
      params_->set_rendering_mode(dragMode_);
      
      behavior_ = Moving;
      lastPos_ = event->pos();
      
      translation_ = ldraw::matrix();
      
      // TODO implement temporary exclusion for visibility extension
      //const QSet<int> *sel = tsset_->getSelection();
      //if (params_->get_rendering_mode() == ldraw_renderer::parameters::model_boundingboxes) {
      //	for (QSet<int>::ConstIterator it = sel->constBegin(); it != sel->constEnd(); ++it)
      //		tvset_->insert(*it);
      //}
    } else {
      if (event->modifiers() == Qt::ControlModifier)
        selectionMethod_ = Addition;
      else if (event->modifiers() == Qt::ShiftModifier)
        selectionMethod_ = Subtraction;
      else if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
        selectionMethod_ = Intersection;
      else
        selectionMethod_ = Normal;
      
      params_->set_rendering_mode(dragMode_);
      
      behavior_ = Dragging;
      region_ = QRect(event->pos(), QSize(0, 0));
      isRegion_ = false;
    }
    
    doneCurrent();
    
    update();
  }
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
  EXIT_IF_NO_DOCUMENT;

  if (behavior_ == Rotating) {
    /* rotating the free viewport */

    (*activeDocument_)->getMouseRotation().move_event(event->pos().x(),
                                                      event->pos().y(),
                                                      width_, height_);
    
    update();
  } else if (behavior_ == Panning) {
    /* moving the viewport */

    Viewport &viewport = (*activeDocument_)->getViewport((int)viewportMode_);
    QPoint p = event->pos() - lastPos_;
    float dx = p.x() / (float)width_  * std::fabs(stretched_.right - stretched_.left);
    float dy = p.y() / (float)height_ * std::fabs(stretched_.bottom - stretched_.top);
    
    viewport.left   -= dx;
    viewport.right  -= dx;
    viewport.top    -= dy;
    viewport.bottom -= dy;
    
    lastPos_ = event->pos();
    
    update();
  } else if (behavior_ == Dragging) {
    /* selecting object(s) */

    region_.setCoords(region_.x(), region_.y(), event->pos().x(), event->pos().y());
    
    if (region_.width() > 3 || region_.height() > 3 ||
        region_.width() < -3 || region_.height() < -3)
      isRegion_ = true;
    
    makeCurrent();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    ldraw_renderer::selection_list result = renderer_->select(
        projectionMatrix_,
        const_cast<float *>(ldraw::matrix().get_pointer()),
        region_.x(), region_.y(),
        region_.width(), region_.height(),
        (*activeDocument_)->getActiveModel(),
        tvset_);
    
    tisset_->setList(result);
    
    doneCurrent();
    
    update();
  } else if (behavior_ == Moving) {
    /* moving object(s) */

    updatePositionVector(event->pos() - lastPos_);
    
    update();
  } else if (behavior_ == MovingByAxis) {
    /* moving object(s) by dragging the anchor */

    ldraw::vector v = unproject(event->pos()) - anchorstart_;
    ldraw::vector n;

    const Editor *editor = Editor::instance();

    if (anchorMode_ == AxisX)
      n = ldraw::vector(editor->snap(v.x()), 0.0f, 0.0f);
    else if (anchorMode_ == AxisY)
      n = ldraw::vector(0.0f, editor->snap(-v.y()), 0.0f);
    else if (anchorMode_ == AxisZ)
      n = ldraw::vector(0.0f, 0.0f, editor->snap(-v.z()));
    else
      return;

    translation_.set_translation_vector(n);

    update();
  } else if (anchorEnabled_) {
    makeCurrent();

    AnchorMode old = anchorHover_;

    if ((anchorHover_ = anchorHitTest(
            event->pos().x(),
            event->pos().y())) != AxisNone ||
        old != AxisNone) {
      doneCurrent();
      update();
      return;
    }

    doneCurrent();
  }
}

void RenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
  EXIT_IF_NO_DOCUMENT;
  
  if ((behavior_ == Rotating && event->button() & Qt::RightButton) ||
      (behavior_ == Panning && event->button() & Qt::MidButton)) {
    behavior_ = Idle;
    params_->set_rendering_mode(renderMode_);
    
    update();
  } else if (behavior_ == Dragging && event->button() & Qt::LeftButton) {
    behavior_ = Idle;
    region_.setCoords(region_.x(), region_.y(), event->pos().x(), event->pos().y());
    
    makeCurrent();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    if (!isRegion_)
      renderer_->set_selection_type(ldraw_renderer::renderer::selection_model_full);
    
    ldraw_renderer::selection_list resultWithDepth = renderer_->select(
        projectionMatrix_,
        const_cast<float *>(ldraw::matrix().get_pointer()),
        region_.x(), region_.y(),
        region_.width(), region_.height(),
        (*activeDocument_)->getActiveModel(),
        tvset_);
    
    if (!isRegion_)
      renderer_->set_selection_type(ldraw_renderer::renderer::selection_points);
    
    params_->set_rendering_mode(renderMode_);
    
    doneCurrent();
    
    std::list<int> result;
    
    int i = 0;
    
    if (!isRegion_ && resultWithDepth.size() > 0) {
      int idx = 0;
      GLuint min = UINT_MAX;
      for (ldraw_renderer::selection_list::iterator it = resultWithDepth.begin();
           it != resultWithDepth.end();
           ++it, ++i) {
        if ((*it).second < min) {
          min = (*it).second;
          idx = (*it).first;
        }
      }
      
      result.push_back(idx);
    } else {
      for (ldraw_renderer::selection_list::iterator it = resultWithDepth.begin();
           it != resultWithDepth.end();
           ++it, ++i)
        result.push_back((*it).first);
    }
    
    emit madeSelection(result, selectionMethod_);
    
    tisset_->clear();
    
    update();
  } else if ((behavior_ == Moving || behavior_ == MovingByAxis) &&
             event->button() & Qt::LeftButton) {
    behavior_ = Idle;
    
    const QSet<int> *sel = tsset_->getSelection();
    if (params_->get_rendering_mode() == ldraw_renderer::parameters::model_boundingboxes) {
      for (QSet<int>::ConstIterator it = sel->constBegin(); it != sel->constEnd(); ++it)
        tvset_->remove(*it);
    }
    
    emit translateObject(translation_);
    
    translation_ = ldraw::matrix();
    
    params_->set_rendering_mode(renderMode_);
    
    update();
  } else if (anchorMode_ != AxisNone) {
    anchorHover_ = AxisNone;
    anchorMode_ = AxisNone;

    update();
  }
}

void RenderWidget::wheelEvent(QWheelEvent *e)
{
  EXIT_IF_NO_DOCUMENT;
  
  Viewport &viewport = (*activeDocument_)->getViewport((int)viewportMode_);
  float centerx = (viewport.left + viewport.right ) * 0.5f;
  float centery = (viewport.top  + viewport.bottom) * 0.5f;
  
  if (e->delta() < 0) {
    // Zoom in
    viewport.left   = centerx - (centerx - viewport.left) * MAGNIFICATION_FACTOR;
    viewport.right  = centerx + (viewport.right - centerx) * MAGNIFICATION_FACTOR;
    viewport.top    = centery - (centery - viewport.top) * MAGNIFICATION_FACTOR;
    viewport.bottom = centery + (viewport.bottom - centery) * MAGNIFICATION_FACTOR;
  } else if (e->delta() > 0) {
    // Zoom out
    viewport.left   = centerx - (centerx - viewport.left) / MAGNIFICATION_FACTOR;
    viewport.right  = centerx + (viewport.right - centerx) / MAGNIFICATION_FACTOR;
    viewport.top    = centery - (centery - viewport.top) / MAGNIFICATION_FACTOR;
    viewport.bottom = centery + (viewport.bottom - centery) / MAGNIFICATION_FACTOR;
  }
  
  update();
}

void RenderWidget::dragEnterEvent(QDragEnterEvent *event)
{
  EXIT_IF_NO_DOCUMENT;
  
  if (viewportMode_ == Free)
    return;
  
  QByteArray data = event->mimeData()->data(RefObject::mimeType);
  
  if (data.isEmpty())
    return;
  
  RefObject refobj = RefObject::deserialize(data);
  
  event->accept();

  translation_ = ldraw::matrix();
  
  behavior_ = Placing;
  objectmetrics_ = refobj.metrics();
  objectmatrix_ = retranslate(tsset_->getLastMatrix());
  anchor_ = objectmatrix_;
  objectcolor_ = tsset_->getLastColor();
  
  params_->set_rendering_mode(dragMode_);
  
  float x = -stretched_.left / (stretched_.right - stretched_.left) * width_;
  float y = stretched_.top  / (stretched_.top - stretched_.bottom) * height_;
  
  lastPos_ = QPoint((int)x, (int)y);
  anchorEnabled_ = true;
  
  update();
}

void RenderWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
  Q_UNUSED(event);
  
  EXIT_IF_NO_DOCUMENT;
  
  behavior_ = Idle;
  params_->set_rendering_mode(renderMode_);

  anchorEnabled_ = false;
  translation_ = ldraw::matrix();

  update();
}

void RenderWidget::dragMoveEvent(QDragMoveEvent *event)
{
  EXIT_IF_NO_DOCUMENT;
  
  updatePositionVector(event->pos() - lastPos_);
  
  update();
}

void RenderWidget::dropEvent(QDropEvent *event)
{
  EXIT_IF_NO_DOCUMENT;
  
  behavior_ = Idle;
  
  event->ignore();
  params_->set_rendering_mode(renderMode_);
  
  updatePositionVector(event->pos() - lastPos_);
  
  QByteArray data = event->mimeData()->data(RefObject::mimeType);
  
  if (data.isEmpty())
    return;
  
  RefObject refobj = RefObject::deserialize(data);
  
  // Cyclic reference test
  ldraw::model *sm = (*activeDocument_)->contents()->
      find_submodel(refobj.filename().toLocal8Bit().data());
  if (sm) {
    if (ldraw::utils::cyclic_reference_test(
            (*activeDocument_)->getActiveModel(), sm)) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot include this part into the current model."));
      
      return;
    }
  }
  
  emit objectDropped(refobj.filename(), translation_ * objectmatrix_, objectcolor_);

  translation_ = ldraw::matrix();
  
  update();
}

}

