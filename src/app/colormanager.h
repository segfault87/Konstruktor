// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _COLORMANAGER_H_
#define _COLORMANAGER_H_

#include <libldr/color.h>

#include <QList>

class KonstruktorColorManager
{
  public:
	KonstruktorColorManager();
	~KonstruktorColorManager();

	void initList();
	const QList<ldraw::color>& colorList() const;

	const QPixmap colorPixmap(const ldraw::color &color);

  private:
	QPixmap generatePixmap(const ldraw::color &color);

  private:
	QList<ldraw::color> colorList_;
};

#endif
