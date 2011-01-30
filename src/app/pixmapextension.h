// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _PIXMAPEXTENSION_H_
#define _PIXMAPEXTENSION_H_

#include <list>
#include <string>

#include <libldr/extension.h>

#include <QPixmap>

namespace Konstruktor
{

class PixmapRenderer;

class PixmapExtension : public ldraw::extension
{
  public:
	PixmapExtension(ldraw::model *m, void *arg = 0L);
	~PixmapExtension();

	const QPixmap& pixmap() const;

	static std::list<ldraw::model *> updateRelevant(ldraw::model *m, PixmapRenderer *renderer);

	static const std::string identifier() { return "pixmap"; }

  private:
	void update();

	QPixmap pixmap_;
};

}

#endif

