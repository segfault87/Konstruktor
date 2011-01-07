// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _PARTITEMS_H_
#define _PARTITEMS_H_

#include <QMetaType>
#include <QString>

#include "refobject.h"

class KonstruktorPartItemBase
{
  public:
	virtual ~KonstruktorPartItemBase() {}
	
	enum Type { TypeCategory, TypePartItem, TypeFavorite };
	
	virtual Type type() = 0;
};

class KonstruktorPartCategory : public KonstruktorPartItemBase
{
  public:
	KonstruktorPartCategory() {}
	KonstruktorPartCategory(const QString &name, int id, int visibility, int idx);

	Type type() { return TypeCategory; }

	const QString& name() const { return name_; }
	int id() const { return id_; }
	int visibility() const { return visibility_; }
	int index() const { return index_; }

  private:
	QString name_;
	int id_;
	int visibility_;
	int index_;
};

class KonstruktorPartItem : public KonstruktorPartItemBase, public KonstruktorRefObject
{
  public:
	KonstruktorPartItem() {}
	KonstruktorPartItem(KonstruktorPartCategory *parent, const QString &desc, const QString &filename, const ldraw::metrics &metrics);

	Type type() { return TypePartItem; }

	const QString& description() const { return desc_; }
	KonstruktorPartCategory* parent() { return parent_; }

  private:
	QString desc_;
	KonstruktorPartCategory *parent_;
};

class KonstruktorFavorites : public KonstruktorPartItemBase
{
  public:
	Type type() { return TypeFavorite; }
};

Q_DECLARE_METATYPE(KonstruktorPartItem);

#endif


