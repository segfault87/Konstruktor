// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _PARTITEMS_H_
#define _PARTITEMS_H_

#include <QMetaType>
#include <QString>

#include "refobject.h"

namespace Konstruktor
{

class PartItemBase
{
  public:
	virtual ~PartItemBase() {}
	
	enum Type { TypeCategory, TypePartItem, TypeFavorite };
	
	virtual Type type() = 0;
};

class PartCategory : public PartItemBase
{
  public:
	PartCategory() {}
	PartCategory(const QString &name, int id, int visibility, int idx);

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

class PartItem : public PartItemBase, public RefObject
{
  public:
	PartItem() {}
	PartItem(PartCategory *parent, const QString &desc, const QString &filename, const ldraw::metrics &metrics);

	Type type() { return TypePartItem; }

	const QString& description() const { return desc_; }
	PartCategory* parent() { return parent_; }

  private:
	QString desc_;
	PartCategory *parent_;
};

class Favorites : public PartItemBase
{
  public:
	Type type() { return TypeFavorite; }
};

}

Q_DECLARE_METATYPE(Konstruktor::PartItem);

#endif


