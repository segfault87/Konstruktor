// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QFont>
#include <QMimeData>
#include <QPixmap>

#include <klocalizedstring.h>

#include <libldr/model.h>

#include "document.h"
#include "pixmapextension.h"
#include "refobject.h"

#include "submodelmodel.h"

KonstruktorSubmodelModel::KonstruktorSubmodelModel(KonstruktorDocument *document, QObject *parent)
	: QAbstractItemModel(parent)
{
	document_ = document;

	resetItems();
}

KonstruktorSubmodelModel::~KonstruktorSubmodelModel()
{
	
}

QPair<std::string, ldraw::model *> KonstruktorSubmodelModel::modelIndexOf(const QModelIndex &index)
{
	if (index.row() == 0)
		return QPair<std::string, ldraw::model *>("", 0L);
	else
		return submodelList_[index.row() - 1];
}

void KonstruktorSubmodelModel::resetItems()
{
	for (std::map<std::string, ldraw::model *>::iterator it = document_->contents()->submodel_list().begin(); it != document_->contents()->submodel_list().end(); ++it) {
		submodelList_.append(QPair<std::string, ldraw::model *>((*it).first, (*it).second));
		ldraw::model *m = (*it).second;
		if (!m->custom_data<ldraw::metrics>())
			m->update_custom_data<ldraw::metrics>();
		
		refobjects_.append(KonstruktorRefObject((*it).first.c_str(), *m->custom_data<ldraw::metrics>()));
	}

	reset();
}

QVariant KonstruktorSubmodelModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) {
		if (index.row() == 0) {
			return i18n("Base model");
		} else {
			return QString("%1\n%2").arg(submodelList_[index.row() - 1].first.c_str(), submodelList_[index.row() - 1].second->desc().c_str());
		}
	} else if (role == Qt::FontRole) {
		if (index.row() == 0) {
			QFont fnt;
			fnt.setBold(true);
			return fnt;
		} else {
			return QVariant();
		}
	} else if (role == Qt::TextAlignmentRole) {
		return (int)(Qt::AlignRight | Qt::AlignVCenter);
	} else if (role == Qt::DecorationRole) {
		ldraw::model *m;
		
		if (index.row() == 0)
			m = document_->contents()->main_model();
		else
			m = submodelList_[index.row() - 1].second;

		return m->custom_data<KonstruktorPixmapExtension>()->pixmap().scaled(96, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	} else if (role == Qt::UserRole) {
		if (index.row() == 0) {
			return "";
		} else {
			return submodelList_[index.row() - 1].first.c_str();
		}
	}

	return QVariant();
}

Qt::ItemFlags KonstruktorSubmodelModel::flags(const QModelIndex &index) const
{
	if (index.row() != 0)
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
	
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex KonstruktorSubmodelModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid())
		return QModelIndex();
	else {
		if (row == 0)
			return createIndex(row, column);
		else
			return createIndex(row, column, (void *)&refobjects_[row - 1]);
	}
}

QVariant KonstruktorSubmodelModel::headerData(int, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return i18n("Submodel");
	else
		return QVariant();
}

int KonstruktorSubmodelModel::rowCount(const QModelIndex &parent) const
{
	if (!document_ || parent.isValid())
		return 0;
	
	return submodelList_.size() + 1;
}

QStringList KonstruktorSubmodelModel::mimeTypes() const
{
	QStringList types;
	types << "application/konstruktor-refobject";
	return types;
}

QMimeData* KonstruktorSubmodelModel::mimeData(const QModelIndexList &indexes) const
{
	if (indexes.size() != 1)
		return 0L;

	const QModelIndex &index = indexes[0];

	if (!index.isValid() || !index.internalPointer())
		return 0L;

	KonstruktorRefObject *item = static_cast<KonstruktorRefObject *>(index.internalPointer());
	
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData = item->serialize();

	mimeData->setData("application/konstruktor-refobject", encodedData);
	return mimeData;
}
