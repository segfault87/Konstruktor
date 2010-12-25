// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#include "pixmaprenderer.h"

#include "pixmapextension.h"

KonstruktorPixmapExtension::KonstruktorPixmapExtension(ldraw::model *m, void *arg)
	: ldraw::extension(m, arg)
{
	pixmap_ = reinterpret_cast<KonstruktorPixmapRenderer *>(m_arg)->renderToPixmap(0L);
}

KonstruktorPixmapExtension::~KonstruktorPixmapExtension()
{

}

const QPixmap& KonstruktorPixmapExtension::pixmap() const
{
	return pixmap_;
}

void KonstruktorPixmapExtension::update()
{
	pixmap_ = reinterpret_cast<KonstruktorPixmapRenderer *>(m_arg)->renderToPixmap(m_model, true);
}

