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

class PartsModel : public QAbstractItemModel
{
	Q_OBJECT;
	
  public:
	PartsModel(QList<PartCategory> &categories, QMap<int, PartCategory *> &categorymap, QMap<int, QList<PartItem> > &list, QObject *parent = 0L);
	virtual ~PartsModel();

	int columnCount(const QModelIndex &) const { return 1; }
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int favoriteRow() const;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant dataCategory(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant dataPart(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant dataFavorite(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;

	QStringList mimeTypes() const;
	QMimeData* mimeData(const QModelIndexList &indexes) const;

  signals:
	void iconViewChanged(const QList<QPair<QString, RefObject> > &list);
	
  private:
	friend class PartsWidget;
	
	Favorites favorites_;

	QList<PartCategory> &categories_;
	QMap<int, PartCategory *> &categorymap_;
	QMap<int, QList<PartItem> > &list_;
};

}


#endif
