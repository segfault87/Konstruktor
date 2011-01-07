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

class KonstruktorRefObject
{
  public:
	KonstruktorRefObject();
	KonstruktorRefObject(const KonstruktorRefObject &rhs);
	KonstruktorRefObject(const QString &filename, const ldraw::metrics &metrics);

	const QString& filename() const;
	const ldraw::metrics& metrics() const;

	void setFilename(const QString &filename);
	void setMetrics(const ldraw::metrics &metrics);

	KonstruktorRefObject& operator= (const KonstruktorRefObject &rhs);

	QByteArray serialize() const;
	static KonstruktorRefObject deserialize(QByteArray &data);

  private:
	QString filename_;
	ldraw::metrics metrics_;
};

Q_DECLARE_METATYPE(KonstruktorRefObject);

#endif
