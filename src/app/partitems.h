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

  const static int kTypePartItem = 0x1;
  const static int kTypeGroup = 0x4;
  const static int kTypeCategory = 0x2;
  const static int kTypeFavorites = 0x3;

  virtual int type() = 0;
};

class PartGroup : public PartItemBase
{
 public:
  virtual ~PartGroup() {}

  virtual int type() { return kTypeGroup; }
};

class PartFavorites : public PartGroup
{
 public:
  virtual ~PartFavorites() {}

  virtual int type() { return PartGroup::type() | kTypeFavorites; }
};

class PartCategory : public PartGroup
{
 public:
  PartCategory() {}
  PartCategory(const QString &name, int id, int visibility, int idx);
  
  int type() { return PartGroup::type() | kTypeCategory; }
  
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
  
  int type() { return kTypePartItem; }
  
  const QString& description() const { return desc_; }
  PartCategory* parent() { return parent_; }
  
 private:
  QString desc_;
  PartCategory *parent_;
};

}

Q_DECLARE_METATYPE(Konstruktor::PartItem);

#endif


