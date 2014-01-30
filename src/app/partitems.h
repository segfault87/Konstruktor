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
  
  enum Type { TypeItem = 0x1, TypeGroup = 0x4, TypeCategory = 0x2, TypeFavorites = 0x3 };
  
  virtual Type type() = 0;
};

class PartGroup : public PartItemBase
{
 public:
  virtual ~PartGroup() {}

  virtual Type type() { return TypeGroup; }
};

class PartFavorites : public PartGroup
{
 public:
  virtual ~PartFavorites() {}

  virtual Type type() { return PartGroup::type() | TypeFavorites; }
};

class PartCategory : public PartGroup
{
 public:
  PartCategory() {}
  PartCategory(const QString &name, int id, int visibility, int idx);
  
  Type type() { return PartGroup::type() | TypeCategory; }
  
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
  PartItem(PartCategory *parent, const QString &desc,
           const QString &filename, const ldraw::metrics &metrics);
  
  Type type() { return TypeItem; }
  
  const QString& description() const { return desc_; }
  PartCategory* parent() { return parent_; }
  
 private:
  QString desc_;
  PartCategory *parent_;
};

}

Q_DECLARE_METATYPE(Konstruktor::PartItem);

#endif


