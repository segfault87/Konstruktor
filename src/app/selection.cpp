// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include "renderwidget.h"
#include "visibilityextension.h"

#include "selection.h"

KonstruktorSelection::KonstruktorSelection()
{
	tsset_ = 0L;
	model_ = 0L;
	visibility_ = 0L;
	inversed_ = false;
}

KonstruktorSelection::~KonstruktorSelection()
{

}


void KonstruktorSelection::setSelection(const QSet<int> &set)
{
	tsset_ = &set;
}

void KonstruktorSelection::setModel(const ldraw::model *m)
{
	model_ = m;
	tsset_ = 0L;
	visibility_ = m->const_custom_data<KonstruktorVisibilityExtension>();
}

void KonstruktorSelection::resetSelection()
{
	tsset_ = 0L;
}

void KonstruktorSelection::setInversed(bool i)
{
	inversed_ = i;
}

const QSet<int>* KonstruktorSelection::getSelection() const
{
	return tsset_;
}

bool KonstruktorSelection::hasSelection() const
{
	return tsset_ != 0L;
}

const ldraw::element_ref* KonstruktorSelection::getLastRef() const
{
	if (tsset_) {
		if (tsset_->count() == 1) {
			int ptr = *tsset_->begin();
			
			const ldraw::element_base *eb = model_->elements()[ptr];
			if (eb->get_type() == ldraw::type_ref)
				return CAST_AS_CONST_REF(eb);
		} else if (tsset_->count() == 0) {
			return 0L;
		} else {
			for (ldraw::model::reverse_iterator it = model_->elements().rbegin(); it != model_->elements().rend(); ++it) {
				if ((*it)->get_type() == ldraw::type_ref)
					return CAST_AS_CONST_REF(*it);
			}
		}
		
	}

	return 0L;
}

ldraw::matrix KonstruktorSelection::getLastMatrix() const
{
	const ldraw::element_ref *ref = getLastRef();

	if (ref)
		return ref->get_matrix();
	else
		return ldraw::matrix();
}

ldraw::color KonstruktorSelection::getLastColor() const
{
	const ldraw::element_ref *ref = getLastRef();

	if (ref)
		return ref->get_color();
	else
		return ldraw::color(0);
}

bool KonstruktorSelection::query(const ldraw::model *, int index, int) const
{
	if (!tsset_)
		return false;

	bool visibility = false;

	if (visibility_)
		visibility = !visibility_->find(index);

	if (inversed_)
		return !(tsset_->contains(index) && visibility);
	else
		return tsset_->contains(index) && visibility;
}

KonstruktorIntermediateSelection::KonstruktorIntermediateSelection(KonstruktorSelection *currentSelection)
	: currentSelection_(currentSelection)
{
	selectionMethod_ = KonstruktorRenderWidget::Normal;
}

KonstruktorIntermediateSelection::~KonstruktorIntermediateSelection()
{

}

void KonstruktorIntermediateSelection::setList(const std::list<std::pair<int, GLuint> > &list)
{
	tsset_.clear();
	
	for (std::list<std::pair<int, GLuint> >::const_iterator it = list.begin(); it != list.end(); ++it)
		tsset_.insert((*it).first);
}

void KonstruktorIntermediateSelection::clear()
{
	tsset_.clear();
}

void KonstruktorIntermediateSelection::setSelectionMethod(int method)
{
	selectionMethod_ = method;
}

bool KonstruktorIntermediateSelection::hasSelection() const
{
	return tsset_.size() > 0;
}

bool KonstruktorIntermediateSelection::query(const ldraw::model *, int index, int) const
{
	if ((selectionMethod_ == KonstruktorRenderWidget::Subtraction || selectionMethod_ == KonstruktorRenderWidget::Intersection) && currentSelection_->hasSelection())
		return !(tsset_.find(index) != tsset_.end() && currentSelection_->getSelection()->contains(index));
	else
		return !(tsset_.find(index) != tsset_.end());
}
