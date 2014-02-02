// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QFont>
#include <QIcon>
#include <QMimeData>
#include <QPixmap>

#include "dbmanager.h"
#include "utils.h"

#include "partsmodel.h"

namespace Konstruktor
{

ItemModelBase::ItemModelBase(QObject *parent)
    : QAbstractItemModel(parent)
{

}

ItemModelBase::~ItemModelBase()
{

}

QVariant ItemModelBase::headerData(int, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return tr("Parts");
  else
    return QVariant();
}

QStringList ItemModelBase::mimeTypes() const
{
  QStringList types;
  types << RefObject::mimeType;
  return types;
}

QMimeData* ItemModelBase::mimeData(const QModelIndexList &indexes) const
{
  if (indexes.size() != 1)
    return 0L;
  
  const QModelIndex &index = indexes[0];
  if (!index.isValid() || !index.internalPointer())
    return 0L;
  
  PartItemBase *item = static_cast<PartItemBase *>(index.internalPointer());
  if (item->type() != PartItemBase::kTypePartItem)
    return 0L;
  
  return dynamic_cast<PartItem *>(item)->mimeData();
}

PartsModel::PartsModel(QList<PartCategory> &categories,
                       QMap<int, PartCategory *> &categorymap,
                       QMap<int, QList<PartItem> > &list,
                       QObject *parent)
    : ItemModelBase(parent), categories_(categories),
      categorymap_(categorymap), list_(list)
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
    return categories_.size() + 1;
  
  PartItemBase *i = static_cast<PartItemBase *>(parent.internalPointer());
  
  if (i->type() == PartItemBase::kTypeCategory) {
    PartCategory *c = dynamic_cast<PartCategory *>(i);
    return list_[c->id()].size();
  } else {
    return 0;
  }
}

QVariant PartsModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid() || !index.internalPointer())
    return QVariant();
  
  switch (static_cast<PartItemBase *>(index.internalPointer())->type()) {
    case PartItemBase::kTypeCategory:
      return dataCategory(index, role);
    case PartItemBase::kTypePartItem:
      return dataPart(index, role);
    case PartItemBase::kTypeFavorites:
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
      return Utils::icon("favorites");
    } else if (role == Qt::FontRole) {
      QFont font;
      font.setBold(true);
      font.setItalic(true);
      return font;
    }
  }
  
  return QVariant();
}

QModelIndex PartsModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();
  
  void *ptr = 0L;
  
  if (parent.isValid()) {
    PartItemBase *s = static_cast<PartItemBase *>(parent.internalPointer());
    if (s->type() == PartItemBase::kTypeCategory) {
      ptr = (void *)&list_[dynamic_cast<PartCategory *>(s)->id()][row];
    }
  } else {
    ptr = (void *)&categories_[row];
  }
  
  return createIndex(row, column, ptr);
}

QModelIndex PartsModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();
  
  PartItemBase *s = static_cast<PartItemBase *>(index.internalPointer());
  if (s->type() == PartItemBase::kTypeCategory) {
    return QModelIndex();
  } else if (s->type() == PartItemBase::kTypePartItem) {
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
  if (s->type() == PartItemBase::kTypePartItem)
    flags |= Qt::ItemIsDragEnabled;
  
  return flags;  
}

MetaPartsModel::MetaPartsModel(FavoritesModel *favorites,
                               PartsModel *parts,
                               QObject *parent)
    : QAbstractItemModel(parent), favorites_(favorites), parts_(parts)
{
  
}

MetaPartsModel::~MetaPartsModel()
{
  
}

int MetaPartsModel::columnCount(const QModelIndex &parent) const
{
  return 1;
}

int MetaPartsModel::rowCount(const QModelIndex &parent) const
{
  if (!parent.isValid())
    return favorites_->rowCount() + parts_->rowCount();
  else
    return parentModel(parent)->rowCount(parent);
}

QVariant MetaPartsModel::headerData(int column,
                                    Qt::Orientation orientation,
                                    int role) const
{
  return parts_->headerData(column, orientation, role);
}

QVariant MetaPartsModel::data(const QModelIndex &index, int role) const
{
  return parentModel(index)->data(index, role);
}

QModelIndex MetaPartsModel::index(int row, int column,
				  const QModelIndex &parent) const
{
  if (row == parts_->rowCount())
    return favorites_->index(0, column, parent);
  else
    return parts_->index(row, column, parent);
}

QModelIndex MetaPartsModel::parent(const QModelIndex &index) const
{
  return parentModel(index)->parent(index);
}

const QAbstractItemModel* MetaPartsModel::parentModel(const QModelIndex &index) const
{
  if (index.row() == parts_->rowCount())
    return favorites_;
  else
    return parts_;
}

}
