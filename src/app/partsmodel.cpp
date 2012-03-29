// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QFont>
#include <QIcon>
#include <QMimeData>
#include <QPixmap>

#include "dbmanager.h"

#include "partsmodel.h"

namespace Konstruktor
{

PartsModel::PartsModel(QList<PartCategory> &categories, QMap<int, PartCategory *> &categorymap, QMap<int, QList<PartItem> > &list, QObject *parent)
	: QAbstractItemModel(parent), categories_(categories), categorymap_(categorymap), list_(list)
{
}

PartsModel::~PartsModel()
{
  
}

int PartsModel::rowCount(const QModelIndex &parent) const
{
  if (parent.column() > 0)
    return 0;
  
  if (!parent.isValid())
    return categories_.size();
  
  PartItemBase *i = static_cast<PartItemBase *>(parent.internalPointer());
  
  if (i->type() == PartItemBase::TypeCategory) {
    PartCategory *c = dynamic_cast<PartCategory *>(i);
    return list_[c->id()].size();
  } else {
    return 0;
  }
}

int PartsModel::favoriteRow() const
{
  return categories_.size();
}

QVariant PartsModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid() || !index.internalPointer())
    return QVariant();
  
  switch (static_cast<PartItemBase *>(index.internalPointer())->type()) {
    case PartItemBase::TypeCategory:
      return dataCategory(index, role);
    case PartItemBase::TypePartItem:
      return dataPart(index, role);
    case PartItemBase::TypeFavorite:
      return dataFavorite(index, role);
    default:
      return QVariant();
  }
}

QVariant PartsModel::dataCategory(const QModelIndex &index, int role) const
{
  PartCategory *cat = static_cast<PartCategory *>(index.internalPointer());
	
  if (role == Qt::DisplayRole) {
    return tr("%1 (%2)").arg(cat->name()).arg(rowCount(index));
  } else if (role == Qt::FontRole) {
    if (cat->visibility() == 1) {
      QFont font;
      font.setBold(true);
      return font;
    } else {
      return QVariant();
    }
  }
  
  return QVariant();
}

QVariant PartsModel::dataPart(const QModelIndex &index, int role) const
{
  PartItem *obj = static_cast<PartItem *>(index.internalPointer());
  
  if (role == Qt::DisplayRole) {
    return tr("%1").arg(obj->description());
  }
  
  return QVariant();
}

QVariant PartsModel::dataFavorite(const QModelIndex &index, int role) const
{
  if (!index.parent().isValid()) {
    if (role == Qt::DisplayRole) {
      return tr("Favorites");
    } else if (role == Qt::DecorationRole) {
      return QIcon::fromTheme("favorites");
    } else if (role == Qt::FontRole) {
      QFont font;
      font.setBold(true);
      font.setItalic(true);
      return font;
    }
  }
  
  return QVariant();
}

QVariant PartsModel::headerData(int, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return tr("Parts");
  else
    return QVariant();
}

QModelIndex PartsModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();
  
  void *ptr = 0L;
  
  if (parent.isValid()) {
    PartItemBase *s = static_cast<PartItemBase *>(parent.internalPointer());
    if (s->type() == PartItemBase::TypeCategory) {
      ptr = (void *)&list_[dynamic_cast<PartCategory *>(s)->id()][row];
    }
  } else {
    if (row == favoriteRow())
      ptr = (void *)&favorites_;
    else
      ptr = (void *)&categories_[row];
  }
  
  return createIndex(row, column, ptr);
}

QModelIndex PartsModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();
  
  PartItemBase *s = static_cast<PartItemBase *>(index.internalPointer());
  if (s->type() == PartItemBase::TypeCategory) {
    return QModelIndex();
  } else if (s->type() == PartItemBase::TypePartItem) {
    PartItem *c = dynamic_cast<PartItem *>(s);
    return createIndex(c->parent()->index(), 0, (void *)c->parent());
  }
  
  return QModelIndex();
}

Qt::ItemFlags PartsModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return 0;

  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  
  PartItemBase *s = static_cast<PartItemBase *>(index.internalPointer());
  if (s->type() == PartItemBase::TypePartItem)
    flags |= Qt::ItemIsDragEnabled;
  
  return flags;
  
}

QStringList PartsModel::mimeTypes() const
{
  QStringList types;
  types << RefObject::mimeType;
  return types;
}

QMimeData* PartsModel::mimeData(const QModelIndexList &indexes) const
{
  if (indexes.size() != 1)
    return 0L;
  
  const QModelIndex &index = indexes[0];
  if (!index.isValid() || !index.internalPointer())
    return 0L;
  
  PartItemBase *item = static_cast<PartItemBase *>(index.internalPointer());
  if (item->type() != PartItemBase::TypePartItem)
    return 0L;
  
  return dynamic_cast<PartItem *>(item)->mimeData();
}

}
