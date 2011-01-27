// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include "pixmaprenderer.h"

#include "pixmapextension.h"

namespace Konstruktor
{

PixmapExtension::PixmapExtension(ldraw::model *m, void *arg)
	: ldraw::extension(m, arg)
{
	pixmap_ = reinterpret_cast<PixmapRenderer *>(m_arg)->renderToPixmap(0L);
}

PixmapExtension::~PixmapExtension()
{

}

const QPixmap& PixmapExtension::pixmap() const
{
	return pixmap_;
}

void PixmapExtension::update()
{
	pixmap_ = reinterpret_cast<PixmapRenderer *>(m_arg)->renderToPixmap(m_model, true);
}

}
