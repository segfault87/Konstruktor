// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include "renderwidget.h"
#include "visibilityextension.h"

#include "selection.h"

namespace Konstruktor
{

Selection::Selection()
{
	tsset_ = 0L;
	model_ = 0L;
	visibility_ = 0L;
	inversed_ = false;
}

Selection::~Selection()
{

}


void Selection::setSelection(const QSet<int> &set)
{
	tsset_ = &set;
}

void Selection::setModel(const ldraw::model *m)
{
	model_ = m;
	tsset_ = 0L;
	visibility_ = m->const_custom_data<VisibilityExtension>();
}

void Selection::resetSelection()
{
	tsset_ = 0L;
}

void Selection::setInversed(bool i)
{
	inversed_ = i;
}

const QSet<int>* Selection::getSelection() const
{
	return tsset_;
}

bool Selection::hasSelection() const
{
	return tsset_ != 0L;
}

const ldraw::element_ref* Selection::getLastRef() const
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

ldraw::matrix Selection::getLastMatrix() const
{
	const ldraw::element_ref *ref = getLastRef();

	if (ref)
		return ref->get_matrix();
	else
		return ldraw::matrix();
}

ldraw::color Selection::getLastColor() const
{
	const ldraw::element_ref *ref = getLastRef();

	if (ref)
		return ref->get_color();
	else
		return ldraw::color(0);
}

bool Selection::query(const ldraw::model *, int index, int) const
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

IntermediateSelection::IntermediateSelection(Selection *currentSelection)
	: currentSelection_(currentSelection)
{
	selectionMethod_ = RenderWidget::Normal;
}

IntermediateSelection::~IntermediateSelection()
{

}

void IntermediateSelection::setList(const std::list<std::pair<int, GLuint> > &list)
{
	tsset_.clear();
	
	for (std::list<std::pair<int, GLuint> >::const_iterator it = list.begin(); it != list.end(); ++it)
		tsset_.insert((*it).first);
}

void IntermediateSelection::clear()
{
	tsset_.clear();
}

void IntermediateSelection::setSelectionMethod(int method)
{
	selectionMethod_ = method;
}

bool IntermediateSelection::hasSelection() const
{
	return tsset_.size() > 0;
}

bool IntermediateSelection::query(const ldraw::model *, int index, int) const
{
	if ((selectionMethod_ == RenderWidget::Subtraction || selectionMethod_ == RenderWidget::Intersection) && currentSelection_->hasSelection())
		return !(tsset_.find(index) != tsset_.end() && currentSelection_->getSelection()->contains(index));
	else
		return !(tsset_.find(index) != tsset_.end());
}

}
