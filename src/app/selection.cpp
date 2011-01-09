// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include "renderwidget.h"

#include "selection.h"

KonstruktorSelection::KonstruktorSelection()
{
	tsset_ = 0L;
	inversed_ = false;
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

bool KonstruktorSelection::query(const ldraw::model *, int index, int) const
{
	if (!tsset_)
		return false;

	if (inversed_)
		return !tsset_->contains(index);
	else
		return tsset_->contains(index);
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
