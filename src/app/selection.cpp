// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

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

/*void KonstruktorSelection::add(const QSet<int> &set)
{
	if (tsset_)
		tsset_->unite(set);
}

void KonstruktorSelection::subtract(const QSet<int> &set)
{
	if (tsset_)
		tsset_->subtract(set);
}

void KonstruktorSelection::intersect(const QSet<int> &set)
{
	if (tsset_)
		tsset_->intersect(set);
}*/

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
