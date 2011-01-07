// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

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
	
	KonstruktorConfig *config = KonstruktorApplication::self()->config();

	for (QList<int>::ConstIterator it = config->colorList().constBegin(); it != config->colorList().constEnd(); ++it)
		colorList_.append(ldraw::color(*it));
}

const QList<ldraw::color>& KonstruktorColorManager::colorList() const
{
	return colorList_;
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


