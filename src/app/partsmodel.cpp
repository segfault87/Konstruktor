// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QFont>
#include <QMimeData>
#include <QPixmap>

#include <kicon.h>
#include <klocalizedstring.h>

#include "dbmanager.h"

#include "partsmodel.h"

KonstruktorPartsModel::KonstruktorPartsModel(QList<KonstruktorPartCategory> &categories, QMap<int, KonstruktorPartCategory *> &categorymap, QMap<int, QList<KonstruktorPartItem> > &list, QObject *parent)
	: QAbstractItemModel(parent), categories_(categories), categorymap_(categorymap), list_(list)
{
}

KonstruktorPartsModel::~KonstruktorPartsModel()
{
	
}

int KonstruktorPartsModel::rowCount(const QModelIndex &parent) const
{
	if (parent.column() > 0)
		return 0;
	
	if (!parent.isValid())
		return categories_.size();
	
	KonstruktorPartItemBase *i = static_cast<KonstruktorPartItemBase *>(parent.internalPointer());

	if (i->type() == KonstruktorPartItemBase::TypeCategory) {
		KonstruktorPartCategory *c = dynamic_cast<KonstruktorPartCategory *>(i);
		return list_[c->id()].size();
	} else {
		return 0;
	}
}

int KonstruktorPartsModel::favoriteRow() const
{
	return categories_.size();
}

QVariant KonstruktorPartsModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || !index.internalPointer())
		return QVariant();
	
	switch (static_cast<KonstruktorPartItemBase *>(index.internalPointer())->type()) {
		case KonstruktorPartItemBase::TypeCategory:
			return dataCategory(index, role);
		case KonstruktorPartItemBase::TypePartItem:
			return dataPart(index, role);
		case KonstruktorPartItemBase::TypeFavorite:
			return dataFavorite(index, role);
		default:
			return QVariant();
	}
}

QVariant KonstruktorPartsModel::dataCategory(const QModelIndex &index, int role) const
{
	KonstruktorPartCategory *cat = static_cast<KonstruktorPartCategory *>(index.internalPointer());
	
	if (role == Qt::DisplayRole) {
		return i18n("%1 (%2)", cat->name(), rowCount(index));
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

QVariant KonstruktorPartsModel::dataPart(const QModelIndex &index, int role) const
{
	KonstruktorPartItem *obj = static_cast<KonstruktorPartItem *>(index.internalPointer());
	
	if (role == Qt::DisplayRole) {
		return i18n("%1", obj->description());
	}
	
	return QVariant();
}

QVariant KonstruktorPartsModel::dataFavorite(const QModelIndex &index, int role) const
{
	if (!index.parent().isValid()) {
		if (role == Qt::DisplayRole) {
			return i18n("Favorites");
		} else if (role == Qt::DecorationRole) {
			return KIcon("favorites");
		} else if (role == Qt::FontRole) {
			QFont font;
			font.setBold(true);
			font.setItalic(true);
			return font;
		}
	}
	
	return QVariant();
}

QVariant KonstruktorPartsModel::headerData(int, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return i18n("Parts");
	else
		return QVariant();
}

QModelIndex KonstruktorPartsModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	void *ptr = 0L;
	
	if (parent.isValid()) {
		KonstruktorPartItemBase *s = static_cast<KonstruktorPartItemBase *>(parent.internalPointer());
		if (s->type() == KonstruktorPartItemBase::TypeCategory) {
			ptr = (void *)&list_[dynamic_cast<KonstruktorPartCategory *>(s)->id()][row];
		}
	} else {
		if (row == favoriteRow())
			ptr = (void *)&favorites_;
		else
			ptr = (void *)&categories_[row];
	}

	return createIndex(row, column, ptr);
}

QModelIndex KonstruktorPartsModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	KonstruktorPartItemBase *s = static_cast<KonstruktorPartItemBase *>(index.internalPointer());
	if (s->type() == KonstruktorPartItemBase::TypeCategory) {
		return QModelIndex();
	} else if (s->type() == KonstruktorPartItemBase::TypePartItem) {
		KonstruktorPartItem *c = dynamic_cast<KonstruktorPartItem *>(s);
		return createIndex(c->parent()->index(), 0, (void *)c->parent());
	}
	
	return QModelIndex();
}

Qt::ItemFlags KonstruktorPartsModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	
	KonstruktorPartItemBase *s = static_cast<KonstruktorPartItemBase *>(index.internalPointer());
	if (s->type() == KonstruktorPartItemBase::TypePartItem)
		flags |= Qt::ItemIsDragEnabled;
	
	return flags;
	
}

QStringList KonstruktorPartsModel::mimeTypes() const
{
	QStringList types;
	types << "application/konstruktor-refobject";
	return types;
}

QMimeData* KonstruktorPartsModel::mimeData(const QModelIndexList &indexes) const
{
	if (indexes.size() != 1)
		return 0L;

	const QModelIndex &index = indexes[0];
	if (!index.isValid() || !index.internalPointer())
		return 0L;

	KonstruktorPartItemBase *item = static_cast<KonstruktorPartItemBase *>(index.internalPointer());
	if (item->type() != KonstruktorPartItemBase::TypePartItem)
		return 0L;
	
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData = dynamic_cast<KonstruktorPartItem *>(item)->serialize();

	mimeData->setData("application/konstruktor-refobject", encodedData);
	return mimeData;
}
