// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _LIBRARYMODEL_H_
#define _LIBRARYMODEL_H_

#include <QAbstractItemModel>

class KonstruktorLibraryModel : public QAbstractItemModel
{
	Q_OBJECT;

  public:
	KonstruktorLibraryModel(QObject *parent = 0L);
	virtual ~KonstruktorLibraryModel();

	int columnCount(const QModelIndex &) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	QModelIndex index(const QModelIndex &index) const;
	QModelIndex parent(const QModelIndex &index) const;
	
	Qt::ItemFlags flags(const QModelIndex &index) const;
};

#endif
