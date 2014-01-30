// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _PARTSMODEL_H_
#define _PARTSMODEL_H_

#include <QAbstractItemModel>
#include <QList>
#include <QMap>
#include <QPair>

#include "partitems.h"

class QMimeData;

namespace Konstruktor
{

class Document;
class PartsWidget;

class ItemModelBase : public QAbstractItemModel
{
 public:
  ItemModelBase(QObject *parent = 0L);
  virtual ~ItemModelBase();

  int columnCount(const QModelIndex &) const { return 1; }

  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;

  QStringList mimeTypes() const;
  QMimeData* mimeData(const QModelIndexList &indexes) const;
}

class FavoritesModel : public ItemModelBase
{
 public:
  FavoritesModel(QObject *parent = 0L);
  virtual ~FavouritesModel();

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
};

class PartsModel : public ItemModelBase
{
  Q_OBJECT;
	
 public:
  PartsModel(QList<PartCategory> &categories,
             QMap<int, PartCategory *> &categorymap,
             QMap<int, QList<PartItem> > &list,
             QObject *parent = 0L);
  virtual ~PartsModel();
  
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant dataCategory(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant dataPart(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant dataFavorite(const QModelIndex &index, int role = Qt::DisplayRole) const;
  
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &) const;
  
  Qt::ItemFlags flags(const QModelIndex &index) const;
  
 private:
  friend class PartsWidget;
  
  QList<PartCategory> &categories_;
  QMap<int, PartCategory *> &categorymap_;
  QMap<int, QList<PartItem> > &list_;
};

class MetaPartsModel : public QAbstractItemModel
{
 public:
  MetaPartsModel(FavoritesModel *favorites, PartsModel *parts, QObject *parent = 0L);
  virtual ~MetaPartsModel();

  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role = Qt::DisplayRole);
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  
  QModelIndex index(int row,int column, const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &) const;

 private:
  const QAbstractItemModel* parentModel(const QModelIndex &index) const;

 private:
  FavoritesModel *favorites_;
  PartsModel *parts_;
};

}

#endif
