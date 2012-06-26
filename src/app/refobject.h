// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _REFOBJECT_H_
#define _REFOBJECT_H_

#include <QByteArray>
#include <QMetaType>
#include <QPair>
#include <QString>

#include <libldr/math.h>
#include <libldr/metrics.h>

namespace ldraw 
{
class metrics;
}

class QMimeData;

namespace Konstruktor
{

class RefObject
{
 public:
  static const char mimeType[];
  
  RefObject();
  RefObject(const RefObject &rhs);
  RefObject(const QString &filename, const ldraw::metrics &metrics);
  
  const QString& filename() const;
  const ldraw::metrics& metrics() const;
  
  void setFilename(const QString &filename);
  void setMetrics(const ldraw::metrics &metrics);
  
  RefObject& operator= (const RefObject &rhs);
  
  QMimeData* mimeData() const;
  
  QByteArray serialize() const;
  static RefObject deserialize(QByteArray &data);
  
 private:
  QString filename_;
  ldraw::metrics metrics_;
};

}

Q_DECLARE_METATYPE(Konstruktor::RefObject);

#endif
