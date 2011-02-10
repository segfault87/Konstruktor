// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <libldr/filter.h>
#include <libldr/metrics.h>
#include <libldr/model.h>

#include "pivotextension.h"

namespace Konstruktor
{

PivotExtension::PivotExtension()
	: ldraw::extension(0L)
{

}

PivotExtension::PivotExtension(ldraw::model *m, void *arg)
	: ldraw::extension(m, arg)
{
	update();
}

PivotExtension::PivotExtension(ldraw::model *m, const ldraw::filter *filter)
	: ldraw::extension(m, 0L)
{
	updateToCenter(filter);
}

PivotExtension::~PivotExtension()
{

}

void PivotExtension::update()
{
	if (m_arg)
		pivot_ = *reinterpret_cast<ldraw::vector *>(m_arg);
}

void PivotExtension::updateToCenter(const ldraw::filter *filter)
{
	if (!m_model)
		return;

	ldraw::metrics metrics(m_model);
	metrics.update(filter);

	pivot_ = (metrics.min() + metrics.max()) * 0.5f;
}

void PivotExtension::updateToCenter()
{
	if (!m_model)
		return;

	if (!m_model->custom_data<ldraw::metrics>())
		m_model->update_custom_data<ldraw::metrics>();

	ldraw::metrics *m = m_model->custom_data<ldraw::metrics>();
	
	pivot_ = (m->min() + m->max()) * 0.5f;
}

const ldraw::vector& PivotExtension::pivot() const
{
	return pivot_;
}

void PivotExtension::setPivot(const ldraw::vector &v)
{
	pivot_ = v;
}

ldraw::vector PivotExtension::queryPivot(ldraw::model *m, bool center, const ldraw::filter *filter)
{
	ldraw::vector v;

	PivotExtension *e = m->custom_data<PivotExtension>();
	if (e) {
		if (center) {
			if (filter)
				e->updateToCenter(filter);
			else
				e->updateToCenter();
		}

		v = e->pivot();
	} else {
		if (center) {
			PivotExtension pe(m, filter);
			v = pe.pivot();
		}
	}

	return v;
}

}
