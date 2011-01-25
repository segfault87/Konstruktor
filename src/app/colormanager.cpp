// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QtAlgorithms>
#include <QPainter>
#include <QPixmap>
#include <QPixmapCache>

#include "colormanager.h"
#include "config.h"
#include "application.h"

KonstruktorColorManager::KonstruktorColorManager()
{
	initList();
}

KonstruktorColorManager::~KonstruktorColorManager()
{
}

void KonstruktorColorManager::initList()
{
	colorList_.clear();
	
	const QList<int> &clist = KonstruktorApplication::self()->config()->colorList();

	foreach (int i, clist)
		colorList_.append(ldraw::color(i));
}

const QList<ldraw::color>& KonstruktorColorManager::colorList() const
{
	return colorList_;
}

void KonstruktorColorManager::setColorList(const QList<ldraw::color> &clist)
{
	colorList_ = clist;

	/* apply settings */

	QList<int> nclist;
	
	foreach (const ldraw::color &c, clist)
		nclist.append(c.get_id());

	KonstruktorConfig *config = KonstruktorApplication::self()->config();
	config->setColorList(nclist);
}

const QList<KonstruktorColorManager::RecentColorPair>& KonstruktorColorManager::recentlyUsed() const
{
	return recentlyUsed_;
}

void KonstruktorColorManager::hit(const ldraw::color &color)
{
	const int limit = KonstruktorApplication::self()->config()->recentlyUsedColorCount();

	for (QList<KonstruktorColorManager::RecentColorPair>::Iterator it = recentlyUsed_.begin(); it != recentlyUsed_.end(); ++it) {
		if ((*it).first == color) {
			(*it).second += 1;
			qSort(recentlyUsed_.begin(), recentlyUsed_.end(), compare);
			return;
		}
	}

	/* remove least used if list is full */
	if (recentlyUsed_.count() == limit)
		recentlyUsed_.pop_back();

	recentlyUsed_.append(QPair<ldraw::color, int>(color, 1));
}

const QPixmap KonstruktorColorManager::colorPixmap(const ldraw::color &color)
{
	QPixmap pixmap;

	if (!QPixmapCache::find(QString("Color%1").arg(color.get_id()), pixmap)) {
		pixmap = generatePixmap(color);
		QPixmapCache::insert(QString("Color%1").arg(color.get_id()), pixmap);
	}

	return pixmap;
}

QPixmap KonstruktorColorManager::generatePixmap(const ldraw::color &color)
{
	const ldraw::color_entity *entity = color.get_entity();

	QPixmap pm(32, 32);
	pm.fill(Qt::white);

	QPainter painter;
	QPen pen;
	QBrush brush(QColor(entity->rgba[0], entity->rgba[1], entity->rgba[2]));

	pen.setWidth(4);
	if (entity->material == ldraw::material_transparent) {
		pen.setColor(QColor(160, 160, 160));
		brush.setStyle(Qt::Dense5Pattern);
	} else {
		pen.setColor(QColor(0, 0, 0));
	}

	painter.begin(&pm);

	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.setPen(pen);
	painter.setBrush(brush);
	painter.drawRect(1, 1, 30, 30);

	painter.end();

	return pm.scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

bool KonstruktorColorManager::compare(const KonstruktorColorManager::RecentColorPair &i1, const KonstruktorColorManager::RecentColorPair &i2)
{
	return i1.second > i2.second;
}
