// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <libldr/model.h>

#include "visibilityextension.h"

namespace Konstruktor
{

VisibilityExtension::VisibilityExtension(ldraw::model *m, void *arg)
	: ldraw::extension(m, arg)
{

}

VisibilityExtension::~VisibilityExtension()
{

}

const std::set<int>& VisibilityExtension::set() const
{
	return visibilityset_;
}

std::set<int>& VisibilityExtension::set()
{
	return visibilityset_;
}

void VisibilityExtension::replaceSet(const std::set<int> &vs)
{
	visibilityset_ = vs;
}

bool VisibilityExtension::find(int idx) const
{
	return visibilityset_.find(idx) != visibilityset_.end();
}

void VisibilityExtension::insert(int idx)
{
	visibilityset_.insert(idx);
}
	
bool VisibilityExtension::remove(int idx)
{
	if (!find(idx))
		return false;

	visibilityset_.erase(idx);

	return true;
}

void VisibilityExtension::clear()
{
	visibilityset_.clear();
}

bool VisibilityExtension::query(const ldraw::model *m, int index, int depth) const
{
	if (m != m_model || depth != 0)
		return false;

	return find(index);
}

// convenience helper
VisibilityExtension* VisibilityExtension::query(ldraw::model *model)
{
	VisibilityExtension *e = model->custom_data<VisibilityExtension>();

	if (!e)
		e = model->init_custom_data<VisibilityExtension>();

	return e;
}

}

