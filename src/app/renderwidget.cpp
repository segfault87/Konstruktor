// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#include <cmath>

#include <libldr/metrics.h>
#include <libldr/utils.h>

#include <QAction>
#include <QActionGroup>
#include <QFontMetrics>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>

#include <klocale.h>
#include <kmessagebox.h>

#include "document.h"
#include "editor.h"
#include "application.h"
#include "mainwindow.h"
#include "refobject.h"
#include "visibilityextension.h"

#include "renderwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

static const QString viewportNames[VIEWPORT_TYPES] = {
	i18n("Top"),
	i18n("Bottom"),
	i18n("Front"),
	i18n("Back"),
	i18n("Left"),
	i18n("Right"),
	i18n("3D")
};

KonstruktorSelection::KonstruktorSelection()
{
	tsset_ = 0L;
}

KonstruktorSelection::~KonstruktorSelection()
{

}


void KonstruktorSelection::setSelection(const QSet<int> &set)
{
	tsset_ = &set;
}

void KonstruktorSelection::resetSelection()
{
	tsset_ = 0L;
}

const QSet<int>* KonstruktorSelection::getSelection() const
{
	return tsset_;
}

bool KonstruktorSelection::hasSelection() const
{
	return tsset_ != 0L;
}

bool KonstruktorSelection::query(const ldraw::model *, int index, int) const
{
	if (!tsset_)
		return false;
	
	return tsset_->contains(index);
}

KonstruktorRenderWidget::KonstruktorRenderWidget(KonstruktorMainWindow *mainwindow, KonstruktorDocument **document, ViewportMode viewport, QGLContext *context, QGLWidget *shareWidget, QWidget *parent)
	: QGLWidget(context, parent, shareWidget), objectmetrics_(0L)
{
	viewportMode_ = Uninitialized;
	parent_ = mainwindow;

	tmodel_ = 0L;
	tvset_ = 0L;
	tsset_ = new KonstruktorSelection();
	
	activeDocument_ = document;
	behavior_ = Idle;

	readConfig();

	makeCurrent();
	
	params_ = new ldraw_renderer::parameters(*KonstruktorApplication::self()->renderer_params());
	renderer_ = ldraw_renderer::renderer_opengl_factory(params_, ldraw_renderer::renderer_opengl_factory::mode_vbo).create_renderer();

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
}

KonstruktorRenderWidget::~KonstruktorRenderWidget()
{
	delete renderer_;
	delete params_;
}

KonstruktorRenderWidget::ViewportMode KonstruktorRenderWidget::viewportMode() const
{
	return viewportMode_;
}

ldraw::vector KonstruktorRenderWidget::viewportCoordinate(const QPoint &dim) const
{
	float xr = stretched_.left + ((stretched_.right  - stretched_.left) / width())  * dim.x();
	float yr = stretched_.top  + ((stretched_.bottom - stretched_.top)  / height()) * dim.y();
	
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

QPoint KonstruktorRenderWidget::globalCoordinate(const ldraw::vector &vec) const
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

int KonstruktorRenderWidget::depthAxis() const
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

void KonstruktorRenderWidget::modelChanged(ldraw::model *)
{
	tsset_->resetSelection();
}

void KonstruktorRenderWidget::selectionChanged(const QSet<int> &set)
{
	tsset_->setSelection(set);

	update();
}

void KonstruktorRenderWidget::setViewport(ViewportMode mode)
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

void KonstruktorRenderWidget::readConfig()
{
	KonstruktorConfig *conf = KonstruktorApplication::self()->config();
	
	gridResolution_ = conf->gridResolution();
	gridRows_ = conf->gridRows();
	gridColumns_ = conf->gridColumns();
	gridX_ = conf->gridXOffset();
	gridY_ = conf->gridYOffset();
	gridZ_ = conf->gridZOffset();

	highlightColor_ = conf->highlightColor();
	
	switch (conf->dragMode()) {
		case KonstruktorConfig::EnumDragMode::Full:
			dragMode_ = ldraw_renderer::parameters::model_full;
			break;
		case KonstruktorConfig::EnumDragMode::Edges:
			dragMode_ = ldraw_renderer::parameters::model_edges;
			break;
		case KonstruktorConfig::EnumDragMode::BoundingBoxes:
			dragMode_ = ldraw_renderer::parameters::model_boundingboxes;
	}
	
	switch (conf->renderMode()) {
		case KonstruktorConfig::EnumRenderMode::Full:
			renderMode_ = ldraw_renderer::parameters::model_full;
			break;
		case KonstruktorConfig::EnumRenderMode::Edges:
			renderMode_ = ldraw_renderer::parameters::model_edges;
			break;
		case KonstruktorConfig::EnumRenderMode::BoundingBoxes:
			renderMode_ = ldraw_renderer::parameters::model_boundingboxes;
	}
}

void KonstruktorRenderWidget::set3DViewport()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	const KonstruktorViewport *viewport;
	
	if (activeDocument_ && *activeDocument_)
		viewport = &((*activeDocument_)->getViewport((int)viewportMode_));
	else
		viewport = &defaultViewport;
	
	float median, d;
	
	if (std::fabs(viewport->bottom-viewport->top)*((float)width_/height_) >= std::fabs(viewport->right-viewport->left)) {
		median = (viewport->right + viewport->left) * 0.5f;
		d = std::fabs((viewport->right-median)*((float)width_/height_))/viewport->aspectRatio;
		stretched_.left = median - d;
		stretched_.right = median + d;
		stretched_.bottom = viewport->bottom;
		stretched_.top = viewport->top;
		glOrtho(stretched_.left, stretched_.right, stretched_.bottom, stretched_.top, 10000.0f, -10000.0f);
	} else {
		median = (viewport->top + viewport->bottom) * 0.5f;
		d = std::fabs((viewport->bottom-median)/((float)width_/height_))*viewport->aspectRatio;
		stretched_.left = viewport->left;
		stretched_.right = viewport->right;
		stretched_.bottom = median + d;
		stretched_.top = median - d;
		glOrtho(stretched_.left, stretched_.right, stretched_.bottom, stretched_.top, 10000.0f, -10000.0f);
	}

	glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix_);
}

// Map current cursor position to world coordinate
void KonstruktorRenderWidget::updatePositionVector(const QPoint &pos)
{
	const KonstruktorEditor *editor = parent_->editor();

	float dx = pos.x() / (float)width_ * std::fabs(stretched_.right - stretched_.left);
	float dy = pos.y() / (float)height_ * std::fabs(stretched_.bottom - stretched_.top);
		
	switch (viewportMode_) {
		case Top:
			translation_ = ldraw::vector(editor->snap(dx), 0.0f, -editor->snap(dy));
			break;
		case Bottom:
			translation_ = ldraw::vector(editor->snap(dx), 0.0f, editor->snap(dy));
			break;
		case Front:
			translation_ = ldraw::vector(editor->snap(dx), editor->snapYAxis(dy), 0.0f);
			break;
		case Back:
			translation_ = ldraw::vector(-editor->snap(dx), editor->snapYAxis(dy), 0.0f);
			break;
		case Left:
			translation_ = ldraw::vector(0.0f, editor->snapYAxis(dy), -editor->snap(dx));
			break;
		case Right:
			translation_ = ldraw::vector(0.0f, editor->snapYAxis(dy), editor->snap(dx));
			break;
		default:
			break;
	}
}

// Set viewport
void KonstruktorRenderWidget::rotate()
{
	const ldraw::metrics *metrics;
	ldraw::vector vec;
	
	switch (viewportMode_) {
		case Free:
			metrics = (*activeDocument_)->getActiveModel()->custom_data<ldraw::metrics>();
			vec = (metrics->min() + metrics->max()) * 0.5f;
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

void KonstruktorRenderWidget::renderScene()
{
	if(*activeDocument_) {
		ldraw::model *curmodel = (*activeDocument_)->getActiveModel();
		if (curmodel != tmodel_) {
			tmodel_ = curmodel;
			tvset_ = KonstruktorVisibilityExtension::query(curmodel);
		}

		// Set default color and depth
		qglClearColor(KonstruktorApplication::self()->config()->backgroundColor());
		glClearDepth(1.0f);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		if (KonstruktorApplication::self()->config()->multisampling())
			glEnable(GL_MULTISAMPLE);

		glMatrixMode(GL_PROJECTION);
		set3DViewport();

		resizeGL(width(), height());
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		rotate();
		if (params_->get_rendering_mode() == ldraw_renderer::parameters::model_boundingboxes)
			glColor3ub(0, 0, 0);
		renderer_->render(curmodel, tvset_);

		if (behavior_ == Placing) {
			glDisable(GL_DEPTH_TEST);
			qglColor(highlightColor_);
			glLineWidth(2.0f);
			glPushMatrix();
			
			glTranslatef(translation_.x(), translation_.y(), translation_.z());
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			
			renderer_->render_bounding_box(objectmetrics_);
			
			glPopMatrix();
			glEnable(GL_DEPTH_TEST);
		}
		
		if (tsset_->hasSelection()) {
			glDisable(GL_DEPTH_TEST);
			qglColor(highlightColor_);
			glLineWidth(2.0f);
			glPushMatrix();
			
			if (behavior_ == Moving)
				glTranslatef(translation_.x(), translation_.y(), translation_.z());

			const QSet<int> *sel = tsset_->getSelection();
			for (QSet<int>::ConstIterator it = sel->begin(); it != sel->end(); ++it) {
				const ldraw::element_base *e = tmodel_->elements()[*it];
				if (e->get_type() == ldraw::type_ref) {
					const ldraw::element_ref *r = CAST_AS_CONST_REF(e);
					if (r->get_model()) {
						glPushMatrix();
						glMultMatrixf(r->get_matrix().transpose().get_pointer());
						if (!r->get_model()->custom_data<ldraw::metrics>())
							r->get_model()->update_custom_data<ldraw::metrics>();
						renderer_->render_bounding_box(*r->get_model()->custom_data<ldraw::metrics>());
						glPopMatrix();
					}
				}
			}

			glPopMatrix();

			glEnable(GL_DEPTH_TEST);
		}

		glEnable(GL_BLEND);

		glDisable(GL_LIGHTING);
		
		if (KonstruktorApplication::self()->config()->drawGrids())
			renderGrid(gridResolution_, gridResolution_, gridRows_, gridColumns_, gridX_, gridY_, gridZ_);
	}
}

void KonstruktorRenderWidget::renderGrid(float xg, float yg, int xc, int yc, float xo, float yo, float zo)
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

void KonstruktorRenderWidget::initializeGL()
{
	// libldr_renderer initialize proc
	renderer_->setup();

	glEnable(GL_MULTISAMPLE);
}

void KonstruktorRenderWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	width_ = width;
	height_ = height;
	length_ = std::sqrt(pow((double)width_, 2.0) + pow((double)height_, 2.0));
}

void KonstruktorRenderWidget::paintEvent(QPaintEvent *)
{
	makeCurrent();

	QPainter p;
	p.begin(this);
	
	/* disable shading if not free view */
	if (viewportMode_ != Free)
		params_->set_shading(false);

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	
	renderScene();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();

	glClear(GL_DEPTH_BUFFER_BIT);
	
	p.setRenderHint(QPainter::Antialiasing);
	
	// Viewport name
	if (behavior_ == Dragging) {
		p.setBrush(QBrush(QColor(51, 115, 186, 160)));
		p.setPen(QPen(QColor(25, 59, 93)));
		p.drawRect(region_);
	}
	
	p.setBrush(QBrush(QColor(24, 24, 24, 160)));
	p.setPen(Qt::NoPen);
	p.drawRect(20, 20, textSize_.width()+10, textSize_.height()+10);
	p.setPen(Qt::white);
	p.drawText(25, 25, textSize_.width(), textSize_.height(), Qt::AlignVCenter, viewportName_);
	
	p.end();

	doneCurrent();
}

void KonstruktorRenderWidget::mousePressEvent(QMouseEvent *event)
{
	// Cancel select
	if (behavior_ != Idle && event->button() & Qt::RightButton) {
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
	
	if (event->button() & Qt::MidButton) {
		params_->set_rendering_mode(dragMode_);
		behavior_ = Panning;
		lastPos_ = event->pos();
		
		update();
	} else if (viewportMode_ == Free && event->button() & Qt::RightButton) {
		params_->set_rendering_mode(dragMode_);
		behavior_ = Rotating;

		(*activeDocument_)->getMouseRotation().press_event(event->pos().x(), event->pos().y(), width_, height_);
		
		update();
	} else if (event->button() & Qt::LeftButton) {
		// convert from QSet to std::set. wtf!!!

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		rotate();
		float matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		if (viewportMode_ != Free && tsset_->hasSelection() && tsset_->getSelection()->size() > 0 && renderer_->hit_test(projectionMatrix_, matrix, event->pos().x(), event->pos().y(), 1, 1, (*activeDocument_)->getActiveModel(), tsset_)) {
			params_->set_rendering_mode(dragMode_);
			
			behavior_ = Moving;
			lastPos_ = event->pos();

			translation_ = ldraw::vector(0.0f, 0.0f, 0.0f);

			const QSet<int> *sel = tsset_->getSelection();
			if (params_->get_rendering_mode() == ldraw_renderer::parameters::model_boundingboxes) {
				for (QSet<int>::ConstIterator it = sel->constBegin(); it != sel->constEnd(); ++it)
					tvset_->insert(*it);
			}
		} else {
			params_->set_rendering_mode(ldraw_renderer::parameters::model_boundingboxes);
			
			behavior_ = Dragging;
			region_ = QRect(event->pos(), QSize(0, 0));
		}
		
		update();
	}
}

void KonstruktorRenderWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (behavior_ == Rotating) {
		(*activeDocument_)->getMouseRotation().move_event(event->pos().x(), event->pos().y(), width_, height_);
		
		update();
	} else if (behavior_ == Panning) {
		KonstruktorViewport &viewport = (*activeDocument_)->getViewport((int)viewportMode_);
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
		region_.setCoords(region_.x(), region_.y(), event->pos().x(), event->pos().y());
		
		update();
	} else if (behavior_ == Moving) {
		updatePositionVector(event->pos() - lastPos_);

		update();
	}
}

void KonstruktorRenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if ((behavior_ == Rotating && event->button() & Qt::RightButton) || (behavior_ == Panning && event->button() & Qt::MidButton)) {
		behavior_ = Idle;
		params_->set_rendering_mode(renderMode_);
		
		update();
	} else if (behavior_ == Dragging && event->button() & Qt::LeftButton) {
		behavior_ = Idle;
		region_.setCoords(region_.x(), region_.y(), event->pos().x(), event->pos().y());

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		rotate();
		float matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		std::list<int> result = renderer_->select(projectionMatrix_, matrix, region_.x(), region_.y(), region_.width(), region_.height(), (*activeDocument_)->getActiveModel(), tvset_);
		
		params_->set_rendering_mode(renderMode_);

		emit madeSelection(result);

		update();
	} else if (behavior_ == Moving && event->button() & Qt::LeftButton) {
		behavior_ = Idle;

		const QSet<int> *sel = tsset_->getSelection();
		if (params_->get_rendering_mode() == ldraw_renderer::parameters::model_boundingboxes) {
			for (QSet<int>::ConstIterator it = sel->constBegin(); it != sel->constEnd(); ++it)
				tvset_->remove(*it);
		}

		if (translation_.length() > LDR_EPSILON)
			emit translateObject(translation_);

		params_->set_rendering_mode(renderMode_);

		update();
	}
}

void KonstruktorRenderWidget::wheelEvent(QWheelEvent *e)
{
	if (!(*activeDocument_)) return;
	
	KonstruktorViewport &viewport = (*activeDocument_)->getViewport((int)viewportMode_);
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

void KonstruktorRenderWidget::dragEnterEvent(QDragEnterEvent *event)
{
	if (viewportMode_ == Free)
		return;
	
	QByteArray data = event->mimeData()->data("application/konstruktor-refobject");
	
	if (data.isEmpty())
		return;

	KonstruktorRefObject refobj = KonstruktorRefObject::deserialize(data);

	event->accept();

	behavior_ = Placing;
	objectmetrics_ = refobj.metrics();
	
	params_->set_rendering_mode(dragMode_);

	float x = -stretched_.left / (stretched_.right - stretched_.left) * width_;
	float y = stretched_.top  / (stretched_.top - stretched_.bottom) * height_;

	lastPos_ = QPoint((int)x, (int)y);

	update();
}

void KonstruktorRenderWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
	behavior_ = Idle;
	params_->set_rendering_mode(renderMode_);

	update();
}

void KonstruktorRenderWidget::dragMoveEvent(QDragMoveEvent *event)
{
	updatePositionVector(event->pos() - lastPos_);

	update();
}

void KonstruktorRenderWidget::dropEvent(QDropEvent *event)
{
	behavior_ = Idle;
	
	event->ignore();
	params_->set_rendering_mode(renderMode_);

	updatePositionVector(event->pos() - lastPos_);

	QByteArray data = event->mimeData()->data("application/konstruktor-refobject");
	
	if (data.isEmpty())
		return;

	KonstruktorRefObject refobj = KonstruktorRefObject::deserialize(data);
	ldraw::matrix matrix;
	matrix.set_translation_vector(translation_);

	// Cyclic reference test
	ldraw::model *sm = (*activeDocument_)->contents()->find_submodel(refobj.filename().toLocal8Bit().data());
	if (sm) {
		if (ldraw::utils::cyclic_reference_test((*activeDocument_)->getActiveModel(), sm)) {
			KMessageBox::error(this, i18n("Cannot include this part into the current model."));
			
			return;
		}
	}
	
	emit objectDropped(refobj.filename(), matrix, ldraw::color(0));
	
	update();
}


#include "renderwidget.moc"
