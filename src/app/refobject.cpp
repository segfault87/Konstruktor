// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <libldr/metrics.h>

#include <QDataStream>
#include <QMimeData>

#include "refobject.h"

namespace Konstruktor
{

const char RefObject::mimeType[] = "application/x-konstruktor-refobject";

RefObject::RefObject()
    : metrics_(0L)
{
}

RefObject::RefObject(const RefObject &rhs)
    : filename_(rhs.filename()), metrics_(rhs.metrics())
{
}

RefObject::RefObject(const QString &filename, const ldraw::metrics &metrics)
    : filename_(filename), metrics_(metrics)
{
}

const QString& RefObject::filename() const
{
  return filename_;
}
	
const ldraw::metrics& RefObject::metrics() const
{
  return metrics_;
}

void RefObject::setFilename(const QString &filename)
{
  filename_ = filename;
}

void RefObject::setMetrics(const ldraw::metrics &metrics)
{
  metrics_ = metrics;
}

RefObject& RefObject::operator= (const RefObject &rhs)
{
  filename_ = rhs.filename();
  metrics_ = rhs.metrics();

  return *this;
}

QMimeData* RefObject::mimeData() const
{
  QMimeData *mime = new QMimeData();
  
  mime->setData(mimeType, serialize());
  
  return mime;
}

QByteArray RefObject::serialize() const
{
  QByteArray encodedData;
  
  QDataStream stream(&encodedData, QIODevice::WriteOnly);
  stream << metrics_.min_().x() << metrics_.min_().y() << metrics_.min_().z();
  stream << metrics_.max_().x() << metrics_.max_().y() << metrics_.max_().z();
  stream << filename();
  
  return encodedData;
}

RefObject RefObject::deserialize(QByteArray &data)
{
  RefObject obj;
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

}
