// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QFont>
#include <QMimeData>
#include <QPixmap>

#include <klocalizedstring.h>

#include <libldr/model.h>

#include "application.h"
#include "config.h"
#include "document.h"
#include "pixmapextension.h"
#include "refobject.h"

#include "submodelmodel.h"

namespace Konstruktor
{

SubmodelModel::SubmodelModel(Document *document, QObject *parent)
	: QAbstractItemModel(parent)
{
	document_ = document;
	active_ = 0;

	resetItems();
}

SubmodelModel::~SubmodelModel()
{
	
}

Document* SubmodelModel::getDocument()
{
	return document_;
}

QPair<std::string, ldraw::model *> SubmodelModel::modelIndexOf(const QModelIndex &index)
{
	if (index.row() == 0)
		return QPair<std::string, ldraw::model *>("", document_->contents()->main_model());
	else
		return submodelList_[index.row() - 1];
}

QModelIndex SubmodelModel::index(const ldraw::model *m)
{
	typedef QPair<std::string, ldraw::model *> NameModelPair;

	if (m == document_->contents()->main_model())
		return index(0, 0);

	int i = 1;
	foreach (const NameModelPair &p, submodelList_) {
		if (p.second == m)
			return index(i, 0);

		++i;
	}

	return QModelIndex();
}

void SubmodelModel::resetItems()
{
	active_ = 0;

	submodelList_.clear();
	refobjects_.clear();
	
	for (std::map<std::string, ldraw::model *>::iterator it = document_->contents()->submodel_list().begin(); it != document_->contents()->submodel_list().end(); ++it) {
		submodelList_.append(QPair<std::string, ldraw::model *>((*it).first, (*it).second));
		ldraw::model *m = (*it).second;
		if (!m->custom_data<ldraw::metrics>())
			m->update_custom_data<ldraw::metrics>();
		
		refobjects_.append(RefObject((*it).first.c_str(), *m->custom_data<ldraw::metrics>()));
	}

	reset();
}

QVariant SubmodelModel::data(const QModelIndex &index, int role) const
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

		return m->custom_data<PixmapExtension>()->pixmap();
	} else if (role == Qt::UserRole) {
		if (index.row() == 0) {
			return "";
		} else {
			return submodelList_[index.row() - 1].first.c_str();
		}
	} else if (role == Qt::BackgroundRole) {
		if (index.row() == active_)
			return Application::self()->config()->activeSubmodelColor();
		else
			return QVariant();
	}
	

	return QVariant();
}

bool SubmodelModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid())
		return false;
	
	if (role == Qt::EditRole) {
		int v = value.toInt();
		
		if (v == 0)
			active_ = -1;
		else
			active_ = index.row();
		
		emit dataChanged(index, index);
		
		return true;
	} else if (role == Qt::DecorationRole) {
		emit dataChanged(index, index);

		return true;
	}

	return false;
}

Qt::ItemFlags SubmodelModel::flags(const QModelIndex &index) const
{
	if (index.row() != 0)
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
	
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex SubmodelModel::index(int row, int column, const QModelIndex &parent) const
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

QVariant SubmodelModel::headerData(int, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return i18n("Submodel");
	else
		return QVariant();
}

int SubmodelModel::rowCount(const QModelIndex &parent) const
{
	if (!document_ || parent.isValid())
		return 0;
	
	return submodelList_.size() + 1;
}

QStringList SubmodelModel::mimeTypes() const
{
	QStringList types;
	types << RefObject::mimeType;
	return types;
}

QMimeData* SubmodelModel::mimeData(const QModelIndexList &indexes) const
{
	if (indexes.size() != 1)
		return 0L;

	const QModelIndex &index = indexes[0];

	if (!index.isValid() || !index.internalPointer())
		return 0L;

	RefObject *item = static_cast<RefObject *>(index.internalPointer());

	return item->mimeData();
}

}
