// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <algorithm>
#include <cmath>
#include <sstream>

#include <libldr/metrics.h>
#include <libldr/part_library.h>
#include <libldr/reader.h>
#include <libldr/utils.h>
#include <libldr/writer.h>

#include <QUndoStack>

#include "editor.h"
#include "application.h"
#include "pixmapextension.h"
#include "pixmaprenderer.h"
#include "submodelmodel.h"
#include "undostackextension.h"
#include "visibilityextension.h"

#include "document.h"

#define INCLUDED_IN_CURRENT_DOCUMENT(m) (m->is_submodel_of(modelBase_) || m == modelBase_->main_model())

namespace Konstruktor
{

// Creates an empty document
Document::Document(const QString &name, const QString &desc, const QString &author, QObject *parent)
	: QObject(parent)
{
	activeUndoStack_ = 0L;
	canSave_ = false;
	
	modelBase_ = new ldraw::model_multipart;
	modelBase_->main_model()->set_name(std::string(name.toLocal8Bit().data()));
	modelBase_->main_model()->set_author(std::string(author.toLocal8Bit().data()));
	modelBase_->main_model()->set_desc(std::string(desc.toLocal8Bit().data()));
	modelBase_->main_model()->update_custom_data<ldraw::metrics>();
	modelBase_->main_model()->init_custom_data<UndoStackExtension>(this);
	
	setActiveModel(modelBase_->main_model());
	
	updatePixmap();
	
	recalibrateScreenDimension();

	model_ = new SubmodelModel(this, this);
}

// Load an existing model from a file
Document::Document(const QString &path, const KUrl &url, QObject *parent)
	: QObject(parent)
{
	activeUndoStack_ = 0L;
	location_ = url;
	canSave_ = false;
	
	ldraw::reader r;
	modelBase_ = r.load_from_file(path.toLocal8Bit().data());
	Application::self()->library()->link(modelBase_);
	
	ldraw::model *mainModel = modelBase_->main_model();
	ldraw::utils::validate_bowtie_quads(mainModel);

	mainModel->update_custom_data<ldraw::metrics>();

	mainModel->init_custom_data<UndoStackExtension>(this);
	for (ldraw::model_multipart::submodel_iterator it = contents()->submodel_list().begin(); it != contents()->submodel_list().end(); ++it)
		(*it).second->init_custom_data<UndoStackExtension>(this);

	setActiveModel(mainModel);
	
	updatePixmap();
	recalibrateScreenDimension();

	model_ = new SubmodelModel(this, this);
}

Document::~Document()
{
	Application::self()->library()->unlink(modelBase_);

	if (modelBase_)
		delete modelBase_;
}

void Document::sendSignals()
{
	emit undoStackAdded(modelBase_->main_model()->custom_data<UndoStackExtension>());
	for (ldraw::model_multipart::submodel_iterator it = contents()->submodel_list().begin(); it != contents()->submodel_list().end(); ++it)
		emit undoStackAdded((*it).second->custom_data<UndoStackExtension>());
}

QByteArray Document::save(bool forceMultipart) const
{
	std::ostringstream stream;
	ldraw::writer writer(stream);

	if (!modelBase_->submodel_list().size() && !forceMultipart)
		writer.write(modelBase_->main_model());
	else
		writer.write(modelBase_);

	return QByteArray(stream.str().c_str());
}

bool Document::setActiveModel(ldraw::model *m)
{
	if (!INCLUDED_IN_CURRENT_DOCUMENT(m))
	  return false;

	activeModel_ = m;
	activeUndoStack_ = activeModel_->custom_data<UndoStackExtension>();
	
	emit undoStackChanged(activeUndoStack_);

	return true;
}

bool Document::setActiveModel(const std::string &key)
{
	ldraw::model *m = modelBase_->find_submodel(key);
	
	if (!m)
		return false;

	return setActiveModel(m);
}

void Document::setSaveable(bool s)
{
	canSave_ = s;
}

ldraw::model* Document::newSubmodel(const std::string &name, const std::string &desc, const std::string &author)
{
	ldraw::model *m = new ldraw::model(desc, name, author, contents());

	if (!modelBase_->insert_submodel(m))
		return 0L;
	
	m->init_custom_data<ldraw::metrics>();
	m->update_custom_data<PixmapExtension>(Application::self()->pixmapRenderer());
	UndoStackExtension *ext = m->init_custom_data<UndoStackExtension>(this);
	emit undoStackAdded(ext);

	model_->resetItems();
	
	return m;
}

void Document::deleteSubmodel(ldraw::model *model)
{
	modelBase_->remove_submodel(model);
}

void Document::updatePixmap()
{
	PixmapRenderer *pr = Application::self()->pixmapRenderer();
	
	modelBase_->main_model()->update_custom_data<PixmapExtension>(pr);
	for (ldraw::model_multipart::submodel_iterator it = contents()->submodel_list().begin(); it != contents()->submodel_list().end(); ++it)
		(*it).second->update_custom_data<PixmapExtension>(pr);
}

bool Document::updatePixmap(ldraw::model *model)
{
	if (!INCLUDED_IN_CURRENT_DOCUMENT(model))
		return false;
	
	model->update_custom_data<PixmapExtension>(Application::self()->pixmapRenderer());

	return true;
}

QUndoStack* Document::activeUndoStack()
{
	return activeUndoStack_;
}

QList<QUndoStack *> Document::undoStacks()
{
	QList<QUndoStack *> undoStacks;

	undoStacks << modelBase_->main_model()->custom_data<UndoStackExtension>();
	for (ldraw::model_multipart::submodel_iterator it = contents()->submodel_list().begin(); it != contents()->submodel_list().end(); ++it)
		undoStacks << (*it).second->custom_data<UndoStackExtension>();

	return undoStacks;
}

void Document::recalibrateScreenDimension()
{
	if (activeModel_->elements().size() == 0) {
		for (int i = 0; i < 7; i++)
			viewport_[i] = defaultViewport;
	} else {
		int count = 0;
		
		for (ldraw::model::const_iterator it = activeModel_->elements().begin(); it != activeModel_->elements().end(); ++it) {
			ldraw::type t = (*it)->get_type();
			if (t == ldraw::type_line || t == ldraw::type_triangle || t == ldraw::type_quadrilateral || t == ldraw::type_condline)
				++count;
			else if (t == ldraw::type_ref) {
				ldraw::element_ref *e = CAST_AS_REF(*it);
				if (e->get_model())
					++count;
			}
		}
		
		// If current model has actual content
		if (count) {
			// Update bounding box
			activeModel_->update_custom_data<ldraw::metrics>();
			const ldraw::metrics *metrics = activeModel_->custom_data<ldraw::metrics>();
			const ldraw::vector &min = metrics->min();
			const ldraw::vector &max = metrics->max();
			
			center_ = (min+max) * 0.5f;
			length_ = ldraw::vector(fabs(max.x()-min.x()), fabs(max.y()-min.y()), fabs(max.z()-min.z()));
			
			float xlen = length_.x()*0.5f;
			float ylen = length_.y()*0.5f;
			float zlen = length_.z()*0.5f;
			
			/* Coordinate data of following viewpoint:
			 * 0: Top
			 * 1: Bottom
			 * 2: Front
			 * 3: Back
			 * 4: Left
			 * 5: Right
			 * 6: Free
			 */
			
			viewport_[0].left   =  min.x() - (xlen*0.1f);
			viewport_[0].right  =  max.x() + (xlen*0.1f);
			viewport_[0].top    = -max.z() - (zlen*0.1f);
			viewport_[0].bottom = -min.z() + (zlen*0.1f);
			viewport_[0].aspectRatio = length_.x()/length_.z();
			
			viewport_[1].left   = min.x() - (xlen*0.1f);
			viewport_[1].right  = max.x() + (xlen*0.1f);
			viewport_[1].top    = min.z() - (zlen*0.1f);
			viewport_[1].bottom = max.z() + (zlen*0.1f);
			viewport_[1].aspectRatio = length_.x()/length_.z();
			
			viewport_[2].left   = min.x() - (xlen*0.1f);
			viewport_[2].right  = max.x() + (xlen*0.1f);
			viewport_[2].top    = min.y() - (ylen*0.1f);
			viewport_[2].bottom = max.y() + (ylen*0.1f);
			viewport_[2].aspectRatio = length_.x()/length_.y();
			
			viewport_[3].left   = -max.x() - (xlen*0.1f);
			viewport_[3].right  = -min.x() + (xlen*0.1f);
			viewport_[3].top    =  min.y() - (ylen*0.1f);
			viewport_[3].bottom =  max.y() + (ylen*0.1f);
			viewport_[3].aspectRatio = length_.x()/length_.y();
			
			viewport_[4].left   = -max.z() - (zlen*0.1f);
			viewport_[4].right  = -min.z() + (zlen*0.1f);
			viewport_[4].top    =  min.y() - (ylen*0.1f);
			viewport_[4].bottom =  max.y() + (ylen*0.1f);
			viewport_[4].aspectRatio = length_.z()/length_.y();
			
			viewport_[5].left   = min.z() - (zlen*0.1f);
			viewport_[5].right  = max.z() + (zlen*0.1f);
			viewport_[5].top    = min.y() - (ylen*0.1f);
			viewport_[5].bottom = max.y() + (ylen*0.1f);
			viewport_[5].aspectRatio = length_.z()/length_.y();
			
			/* Getting viewport dimension from 3d-view is a bit expensive task.
			 * Following method takes viewport dimension from precalculated bounding box
			 * instead of every meshes and lines from the model.
			 * Since a bounding box can't represent exact dimension of model from arbitrary viewpoint,
			 * this method is reasonably fast but inaccurate.
			 */
			
			// Initialize with extreme value for comparison
			viewport_[6].left   = 1e30;
			viewport_[6].right  = -1e30;
			viewport_[6].top    = 1e30;
			viewport_[6].bottom = -1e30;
			
			ldraw::vector freeVec[8];
			ldraw::vector vec = (min + max) * 0.5f;
			ldraw::vector tmin = min - vec;
			ldraw::vector tmax = max - vec;
			ldraw::matrix rmat = rotation_.get_matrix();
			
			// Transform
			freeVec[0] = rmat * tmin;
			freeVec[1] = rmat * ldraw::vector(tmin.x(), tmin.y(), tmax.z());
			freeVec[2] = rmat * ldraw::vector(tmin.x(), tmax.y(), tmin.z());
			freeVec[3] = rmat * ldraw::vector(tmin.x(), tmax.y(), tmax.z());
			freeVec[4] = rmat * ldraw::vector(tmax.x(), tmin.y(), tmin.z());
			freeVec[5] = rmat * ldraw::vector(tmax.x(), tmin.y(), tmax.z());
			freeVec[6] = rmat * ldraw::vector(tmax.x(), tmax.y(), tmin.z());
			freeVec[7] = rmat * tmax;
			
			// Compare
			for (int i = 0; i < 8; i++) {
				if (freeVec[i].x() < viewport_[6].left)
					viewport_[6].left = freeVec[i].x();
				if (freeVec[i].x() > viewport_[6].right)
					viewport_[6].right = freeVec[i].x();
				if (freeVec[i].y() > viewport_[6].bottom)
					viewport_[6].bottom = freeVec[i].y();
				if (freeVec[i].y() < viewport_[6].top)
					viewport_[6].top = freeVec[i].y();
			}
			
			float fxlen = fabs(viewport_[6].right  - viewport_[6].left)*0.5f; 
			float fylen = fabs(viewport_[6].bottom - viewport_[6].top )*0.5f;
			
			// Update
			viewport_[6].left   -= fxlen*0.1f;
			viewport_[6].right  += fxlen*0.1f;
			viewport_[6].top    -= fylen*0.1f;
			viewport_[6].bottom += fylen*0.1f;
			viewport_[6].aspectRatio = fxlen/fylen;
		} else {
			for (int i = 0; i < 7; i++)
				viewport_[i] = defaultViewport;
		}
	}
}

void Document::resetPerspective()
{
	rotation_ = ldraw_renderer::mouse_rotation();
}

}
