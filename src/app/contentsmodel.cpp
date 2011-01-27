// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QBrush>

#include <kicon.h>
#include <klocale.h>

#include <libldr/bfc.h>
#include <libldr/elements.h>
#include <libldr/model.h>

#include "colormanager.h"
#include "document.h"
#include "application.h"
#include "visibilityextension.h"

#include "contentsmodel.h"

namespace Konstruktor
{

ContentsModel::ContentsModel(QObject *parent)
	: QAbstractItemModel(parent)
{
	model_ = 0L;
	checkTable_ = 0L;

	connect(this, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(dataChanged(const QModelIndex &, const QModelIndex &)));
}

ContentsModel::~ContentsModel()
{
	
}

void ContentsModel::setDocument(Document *document)
{
	if (document == 0L) {
		model_ = 0L;
		checkTable_ = 0L;
	} else {
		model_ = document->getActiveModel();
		checkTable_ = VisibilityExtension::query(model_);
	}

	reset();
}

int ContentsModel::rowCount(const QModelIndex &parent) const
{
	if (model_ && !parent.isValid())
		return model_->elements().size();
	else
		return 0;
}

QVariant ContentsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal) {
		if (role == Qt::DisplayRole) {
			switch (section) {
				case ColumnIndex:
					return i18n("No.");
				case ColumnObjectType:
					return i18n("Type");
				case ColumnPosition:
					return i18n("Position");
				case ColumnData:
					return i18n("Data");
				default:
					return QVariant();
			}
		} else if (role == Qt::DecorationRole && section == ColumnCheck) {
			return KIcon("edit-delete");
		}
	}

	return QVariant();
}

QVariant ContentsModel::data(const QModelIndex &index, int role) const
{
	ldraw::element_base *b = model_->elements()[index.row()];
	
	if (role == Qt::DisplayRole) {
		switch (index.column()) {
			case ColumnIndex:
				return QVariant(QString::number(index.row()));
			case ColumnObjectType:
				switch (b->get_type()) {
					case ldraw::type_comment:
						return i18n("Comment");
					case ldraw::type_state:
						return i18n("Meta");
					case ldraw::type_print:
						return i18n("Print");
					case ldraw::type_ref:
					{
						const ldraw::element_ref *cref = CAST_AS_CONST_REF(b);
						if (!cref->get_model())
							return i18n("Unresolved");
						else if (cref->get_model()->modeltype() == ldraw::model::submodel)
							return i18n("Submodel");
						else if (cref->get_model()->modeltype() == ldraw::model::external_file)
							return i18n("File");
						else
							return i18n("Part");
					}
					case ldraw::type_line:
						return i18n("Line");
					case ldraw::type_triangle:
						return i18n("Triangle");
					case ldraw::type_quadrilateral:
						return i18n("Quadrilateral");
					case ldraw::type_condline:
						return i18n("Conditional Line");
					case ldraw::type_bfc:
						return i18n("BFC Control");
					default:
						return QVariant();
				}
			case ColumnPosition:
				if (b->get_type() == ldraw::type_ref) {
					const ldraw::matrix &matrix = CAST_AS_CONST_REF(b)->get_matrix();
					return i18n("%1, %2, %3", matrix.value(0, 3), matrix.value(1, 3), matrix.value(2, 3));
				} else {
					return i18n("-");
				}
			case ColumnData:
				switch (b->get_type()) {
					case ldraw::type_comment:
					{
						const ldraw::element_comment *e = CAST_AS_CONST_COMMENT(b);
						return e->get_comment().c_str();
					}
					case ldraw::type_state:
					{
						const ldraw::element_state *e = CAST_AS_CONST_STATE(b);
						switch (e->get_state()) {
							case ldraw::element_state::state_step:
								return i18n("Step");
							case ldraw::element_state::state_pause:
								return i18n("Pause");
							case ldraw::element_state::state_clear:
								return i18n("Clear");
							case ldraw::element_state::state_save:
								return i18n("Pause");
							default:
								return QVariant();
						}
					}
					case ldraw::type_print:
					{
						const ldraw::element_print *e = CAST_AS_CONST_PRINT(b);
						return e->get_string().c_str();
					}
					case ldraw::type_ref:
					{
						const ldraw::element_ref *e = CAST_AS_CONST_REF(b);
						return i18n("%1 (%2)", e->filename().c_str(), e->get_model() ? e->get_model()->desc().c_str() : i18n("Unlinked"));
					}
					case ldraw::type_line:
					{
						const ldraw::element_line *e = CAST_AS_CONST_LINE(b);
						return i18n("(%1, %2, %3) - (%4, %5, %6)", e->pos1().x(), e->pos1().y(), e->pos1().z(), e->pos2().x(), e->pos2().y(), e->pos2().z());
					}
					case ldraw::type_triangle:
					{
						const ldraw::element_triangle *e = CAST_AS_CONST_TRIANGLE(b);
						return i18n("(%1, %2, %3) - (%4, %5, %6) - (%7, %8, %9)", e->pos1().x(), e->pos1().y(), e->pos1().z(), e->pos2().x(), e->pos2().y(), e->pos2().z(), e->pos3().x(), e->pos3().y(), e->pos3().z());
					}
					case ldraw::type_quadrilateral:
					{
						const ldraw::element_quadrilateral *e = CAST_AS_CONST_QUADRILATERAL(b);
						return i18n("(%1, %2, %3) - (%4, %5, %6)", e->pos1().x(), e->pos1().y(), e->pos1().z(), e->pos2().x(), e->pos2().y(), e->pos2().z()) + i18n(" - (%1, %2, %3) - (%4, %5, %6)", e->pos3().x(), e->pos3().y(), e->pos3().z(), e->pos4().x(), e->pos4().y(), e->pos4().z());
					}
					case ldraw::type_condline:
					{
						const ldraw::element_condline *e = CAST_AS_CONST_CONDLINE(b);
						return i18n("Line: (%1, %2, %3) - (%4, %5, %6)", e->pos1().x(), e->pos1().y(), e->pos1().z(), e->pos2().x(), e->pos2().y(), e->pos2().z()) + i18n(", Guide: (%1, %2, %3) - (%4, %5, %6)", e->pos3().x(), e->pos3().y(), e->pos3().z(), e->pos4().x(), e->pos4().y(), e->pos4().z());
					}
					case ldraw::type_bfc:
					{
						ldraw::element_bfc::command c = CAST_AS_BFC(b)->get_command();
						if (c == ldraw::element_bfc::noclip)
							return i18n("No Clip");
						else if (c == ldraw::element_bfc::invertnext)
							return i18n("Invert Next");

						return QString("%1%2").arg(i18n(c & ldraw::element_bfc::clip ? "Clip, " : "")).arg(i18n(c & ldraw::element_bfc::cw ? "Clockwise" : "Counter-Clockwise"));
					}
					default:
						return QVariant();
				}
			default:
				return QVariant();
		}
	} else if (role == Qt::BackgroundRole) {
		switch (b->get_type()) {
			case ldraw::type_comment:
				if (Application::self()->config()->listCommentColorize())
					return QBrush(Application::self()->config()->listCommentColor());
				else
					return QVariant();
			case ldraw::type_state:
			case ldraw::type_print:
				if (Application::self()->config()->listMetaColorize())
					return QBrush(Application::self()->config()->listMetaColor());
				else
					return QVariant();
			case ldraw::type_ref:
				if (Application::self()->config()->listRefColorize()) {
					const ldraw::element_ref *e = CAST_AS_CONST_REF(b);
					if (!e->get_model())
						return QBrush(Application::self()->config()->listUnresolvedColor());
					else if (e->get_model()->is_submodel_of(model_->parent()))
						return QBrush(Application::self()->config()->listSubmodelColor());
					else
						return QBrush(Application::self()->config()->listPartColor());
				} else {
					return QVariant();
				}
			case ldraw::type_line:
			case ldraw::type_triangle:
			case ldraw::type_quadrilateral:
			case ldraw::type_condline:
				if (Application::self()->config()->listPrimitiveColorize())
					return QBrush(Application::self()->config()->listPrimitiveColor());
				else
					return QVariant();
			default:
				return QVariant();
		}
	} else if (role == Qt::TextAlignmentRole) {
		if (index.column() == ColumnIndex || index.column() == ColumnObjectType)
			return Qt::AlignHCenter;
	} else if (role == Qt::DecorationRole) {
		if (index.column() == ColumnData && model_->elements()[index.row()]->capabilities() & ldraw::capability_color) {
			const ldraw::color &c = dynamic_cast<const ldraw::element_colored_base *>(model_->elements()[index.row()])->get_color();
			return ColorManager::colorPixmap(c);
		}
	} else if (role == Qt::CheckStateRole) {
		if (index.column() == ColumnCheck) {
			if (model_->elements()[index.row()]->line_type() == 0)
				return QVariant();
			else if (checkTable_->find(index.row()))
				return Qt::Checked;
			else
				return Qt::Unchecked;
		}
	}

	return QVariant();
}

bool ContentsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.column() == ColumnCheck && role == Qt::CheckStateRole) {
		if (model_->elements()[index.row()]->line_type() == 0)
			return false;
		
		int checkstate = value.toInt();

		if (checkstate == Qt::Unchecked) {
			checkTable_->remove(index.row());
			emit unhide(index);
		} else if (checkstate == Qt::Checked) {
			checkTable_->insert(index.row());
			emit hide(index);
		}

		emit dataChanged(index, ContentsModel::index(index.row(), ColumnCount - 1));
		
		return true;
	}

	return false;
}

QModelIndex ContentsModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid()) {
		return QModelIndex();
	} else {
		if (column == ColumnCheck)
			return createIndex(row, column, (void *)model_->elements()[row]);
		else
			return createIndex(row, column);
	}
}

Qt::ItemFlags ContentsModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flags = Qt::ItemIsEnabled;

	if (!checkTable_->find(index.row()))
		flags |= Qt::ItemIsSelectable;
	
	if (index.column() == ColumnCheck && model_->elements()[index.row()]->get_type() == ldraw::type_ref)
		flags |= Qt::ItemIsUserCheckable;
	
	return flags;
}

void ContentsModel::rowsChanged(const QPair<CommandBase::AffectedRow, QSet<int> > &/*rowList*/)
{
	// FIXME
	reset();

	/*if (!inverse) {
		for (int i = 0; i < rowList.size(); ++i) {
			int row = rowList[i].first;
			if (row == -1)
				row = rowCount();
				
			if (rowList[i].second == CommandBase::Inserted) {
				insertRow(row);
			} else {
				removeRow(row);
			}
		}
	} else {
		for (int i = rowList.size() - 1; i >= 0; --i) {
			int row = rowList[i].first;
			if (row == -1)
				row = rowCount();
			
			if (rowList[i].second == CommandBase::Inserted) {
				removeRow(row);
			} else {
				insertRow(row);
			}
		}
		}*/
}

void ContentsModel::hideSelected(const QSet<int> &selection)
{
	for (QSet<int>::ConstIterator it = selection.constBegin(); it != selection.constEnd(); ++it) {
		if (model_->elements()[*it]->line_type() == 0)
			continue;

		if (!checkTable_->find(*it)) {
			checkTable_->insert(*it);
			
			emit dataChanged(index(*it, 0), index(*it, ColumnCount - 1));
		}
	}
}

void ContentsModel::unhideAll()
{
	checkTable_->clear();

	emit dataChanged(index(0, ColumnIndex), index(rowCount() - 1, ColumnIndex));
}

void ContentsModel::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	// If visibility column has changed
	if (topLeft.column() == ColumnIndex || bottomRight.column() == ColumnIndex)
		emit viewChanged();
}

}
