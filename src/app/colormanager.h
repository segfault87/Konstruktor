// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _COLORMANAGER_H_
#define _COLORMANAGER_H_

#include <libldr/color.h>

#include <QList>
#include <QPair>

class KonstruktorColorManager
{
  public:
	typedef QPair<ldraw::color, int> RecentColorPair;
	
	KonstruktorColorManager();
	~KonstruktorColorManager();

	void initList();
	
	const QList<ldraw::color>& colorList() const;
	void setColorList(const QList<ldraw::color> &clist);

	const QList<QPair<ldraw::color, int> >& recentlyUsed() const;
	void hit(const ldraw::color &color);

	static const QPixmap colorPixmap(const ldraw::color &color);

  private:
	static QPixmap generatePixmap(const ldraw::color &color);
	static bool compare(const RecentColorPair &i1, const RecentColorPair &i2);

  private:
	QList<ldraw::color> colorList_;
	QList<RecentColorPair> recentlyUsed_;
};

#endif
