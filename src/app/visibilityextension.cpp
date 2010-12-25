// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#include <libldr/model.h>

#include "visibilityextension.h"

KonstruktorVisibilityExtension::KonstruktorVisibilityExtension(ldraw::model *m, void *arg)
	: ldraw::extension(m, arg)
{

}

KonstruktorVisibilityExtension::~KonstruktorVisibilityExtension()
{

}

const std::set<int>& KonstruktorVisibilityExtension::set() const
{
	return visibilityset_;
}

std::set<int>& KonstruktorVisibilityExtension::set()
{
	return visibilityset_;
}

void KonstruktorVisibilityExtension::replaceSet(const std::set<int> &vs)
{
	visibilityset_ = vs;
}

bool KonstruktorVisibilityExtension::find(int idx) const
{
	return visibilityset_.find(idx) != visibilityset_.end();
}

void KonstruktorVisibilityExtension::insert(int idx)
{
	visibilityset_.insert(idx);
}
	
bool KonstruktorVisibilityExtension::remove(int idx)
{
	if (!find(idx))
		return false;

	visibilityset_.erase(idx);

	return true;
}

void KonstruktorVisibilityExtension::clear()
{
	visibilityset_.clear();
}

bool KonstruktorVisibilityExtension::query(const ldraw::model *m, int index, int depth) const
{
	if (m != m_model || depth != 0)
		return false;

	return find(index);
}

// convenience helper
KonstruktorVisibilityExtension* KonstruktorVisibilityExtension::query(ldraw::model *model)
{
	KonstruktorVisibilityExtension *e = model->custom_data<KonstruktorVisibilityExtension>();

	if (!e)
		e = model->init_custom_data<KonstruktorVisibilityExtension>();

	return e;
}


