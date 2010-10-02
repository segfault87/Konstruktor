// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#include <libldr/metrics.h>

#include <QDataStream>

#include "refobject.h"

KonstruktorRefObject::KonstruktorRefObject()
	: metrics_(0L)
{
}

KonstruktorRefObject::KonstruktorRefObject(const KonstruktorRefObject &rhs)
	: filename_(rhs.filename()), metrics_(rhs.metrics())
{
}

KonstruktorRefObject::KonstruktorRefObject(const QString &filename, const ldraw::metrics &metrics)
	: filename_(filename), metrics_(metrics)
{
}

const QString& KonstruktorRefObject::filename() const
{
	return filename_;
}
	
const ldraw::metrics& KonstruktorRefObject::metrics() const
{
	return metrics_;
}

void KonstruktorRefObject::setFilename(const QString &filename)
{
	filename_ = filename;
}

void KonstruktorRefObject::setMetrics(const ldraw::metrics &metrics)
{
	metrics_ = metrics;
}

KonstruktorRefObject& KonstruktorRefObject::operator= (const KonstruktorRefObject &rhs)
{
	filename_ = rhs.filename();
	metrics_ = rhs.metrics();

	return *this;
}

QByteArray KonstruktorRefObject::serialize() const
{
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	stream << metrics_.min().x() << metrics_.min().y() << metrics_.min().z();
	stream << metrics_.max().x() << metrics_.max().y() << metrics_.max().z();
	stream << filename();

	return encodedData;
}

KonstruktorRefObject KonstruktorRefObject::deserialize(QByteArray &data)
{
	KonstruktorRefObject obj;
	QString filename;
	ldraw::vector min, max;

	QDataStream stream(&data, QIODevice::ReadOnly);
	
	stream >> min.x() >> min.y() >> min.z();
	stream >> max.x() >> max.y() >> max.z();
	stream >> filename;

	obj.setMetrics(ldraw::metrics(min, max));
	obj.setFilename(filename);
	
	return obj;
}
