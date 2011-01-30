// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <libldr/model.h>
#include <libldr/utils.h>

#include "pixmaprenderer.h"

#include "pixmapextension.h"

namespace Konstruktor
{

PixmapExtension::PixmapExtension(ldraw::model *m, void *arg)
	: ldraw::extension(m, arg)
{
	update();
}

PixmapExtension::~PixmapExtension()
{

}

const QPixmap& PixmapExtension::pixmap() const
{
	return pixmap_;
}

std::list<ldraw::model *> PixmapExtension::updateRelevant(ldraw::model *m, PixmapRenderer *renderer)
{
	m->update_custom_data<PixmapExtension>(renderer);

	if (m->parent()) {
		std::list<ldraw::model *> alist = ldraw::utils::affected_models(m->parent(), m);

		for (std::list<ldraw::model *>::iterator it = alist.begin(); it != alist.end(); ++it)
			(*it)->update_custom_data<PixmapExtension>(renderer);

		return alist;
	}

	return std::list<ldraw::model *>();
}

void PixmapExtension::update()
{
	pixmap_ = reinterpret_cast<PixmapRenderer *>(m_arg)->renderToPixmap(m_model, true);

	if (pixmap_.width() > 96 || pixmap_.height() > 96)
		pixmap_ = pixmap_.scaled(96, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

}
