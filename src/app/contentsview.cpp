// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <libldr/elements.h>

#include "contentsmodel.h"

#include "contentsview.h"

KonstruktorContentsView::KonstruktorContentsView(QWidget *parent)
	: QTreeView(parent)
{
	setRootIsDecorated(false);
	setSelectionBehavior(QTreeView::SelectRows);
	setSelectionMode(QTreeView::ExtendedSelection);

	hiddenIndices_ = 0L;
}

KonstruktorContentsView::~KonstruktorContentsView()
{
	
}

int KonstruktorContentsView::uniqueSelection() const
{
	if (selectedIndices_.size() == 1)
		return *selectedIndices_.begin();

	return -1;
}

void KonstruktorContentsView::hide(const QModelIndex &index)
{
	if (selectedIndices_.contains(index.row())) {
		hiddenIndices_->insert(index.row());
		selectedIndices_.remove(index.row());

		emit selectionChanged(selectedIndices_);
	}
}

void KonstruktorContentsView::unhide(const QModelIndex &index)
{
	if (hiddenIndices_->find(index.row()) != hiddenIndices_->end()) {
		hiddenIndices_->erase(index.row());

		if (selectionModel()->isSelected(index))
			selectedIndices_.insert(index.row());

		emit selectionChanged(selectedIndices_);
	}
}

void KonstruktorContentsView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	repaint();
	
	QTreeView::selectionChanged(selected, deselected);
	
	int affected = 0;

	const QModelIndexList &list = selected.indexes();
	for (QModelIndexList::ConstIterator it = list.begin(); it != list.end(); ++it) {
		if ((*it).column() == 0 && (*it).isValid() && !selectedIndices_.contains((*it).row())) {
			if (static_cast<ldraw::element_base *>((*it).internalPointer())->line_type() == 0)
				continue;
			
			selectedIndices_.insert((*it).row());
			++affected;
		}
	}

	const QModelIndexList &delist = deselected.indexes();
	for (QModelIndexList::ConstIterator it = delist.begin(); it != delist.end(); ++it) {
		if ((*it).column() == 0 && (*it).isValid() && selectedIndices_.contains((*it).row())) {
			if (static_cast<ldraw::element_base *>((*it).internalPointer())->line_type() == 0)
					continue;
			
			if (selectedIndices_.contains((*it).row()))
				selectedIndices_.remove((*it).row());
			else if (hiddenIndices_->find((*it).row()) != hiddenIndices_->end())
				hiddenIndices_->erase((*it).row());

			++affected;
		}
	}
	
	if (affected)
		emit selectionChanged(selectedIndices_);
}

void KonstruktorContentsView::hideSelected()
{
	dynamic_cast<KonstruktorContentsModel *>(model())->hideSelected(selectedIndices_);

	for (QSet<int>::ConstIterator it = selectedIndices_.constBegin(); it != selectedIndices_.constEnd(); ++it)
		hiddenIndices_->insert(*it);
	selectedIndices_.clear();

	emit selectionChanged(selectedIndices_);
}

void KonstruktorContentsView::unhideAll()
{
	dynamic_cast<KonstruktorContentsModel *>(model())->unhideAll();

	selectedIndices_.clear();
	hiddenIndices_->clear();
	QModelIndexList selection = selectionModel()->selection().indexes();
	for (QModelIndexList::Iterator it = selection.begin(); it != selection.end(); ++it)
		selectedIndices_.insert((*it).row());

	emit selectionChanged(selectedIndices_);
}

void KonstruktorContentsView::modelChanged(ldraw::model *m)
{
	model_ = m;
	
	selectedIndices_.clear();

	if (!model_)
		return;
	
	hiddenIndices_ = &KonstruktorVisibilityExtension::query(model_)->set();
}

void KonstruktorContentsView::updateSelection(const std::list<int> &selection, KonstruktorRenderWidget::SelectionMethod method)
{
	const int maxcol = model()->columnCount() - 1;
	
	QItemSelection s;

	if (method == KonstruktorRenderWidget::Intersection) {
		const QItemSelection &cis = selectionModel()->selection();
		
		for (std::list<int>::const_iterator it = selection.begin(); it != selection.end(); ++it) {
			if (cis.contains(model()->index(*it, 0)))
				s.select(model()->index(*it, 0), model()->index(*it, maxcol));
		}
	} else {
		for (std::list<int>::const_iterator it = selection.begin(); it != selection.end(); ++it)
			s.select(model()->index(*it, 0), model()->index(*it, maxcol));
	}

	QItemSelectionModel::SelectionFlag flag = QItemSelectionModel::ClearAndSelect;
	
	if (method == KonstruktorRenderWidget::Addition)
		flag = QItemSelectionModel::Select;
	else if (method == KonstruktorRenderWidget::Subtraction)
		flag = QItemSelectionModel::Deselect;
	
	selectionModel()->select(s, flag);
}

void KonstruktorContentsView::rowsChanged(const QPair<KonstruktorCommandBase::AffectedRow, QSet<int> > &rows)
{
	// TODO preserve visibility
	
	if (!rows.second.size())
		return;
		
	selectedIndices_.clear();
	hiddenIndices_->clear();

	if (rows.first == KonstruktorCommandBase::Removed)
		clearSelection();
	else {
		QItemSelection s;
		for (QSet<int>::ConstIterator it = rows.second.constBegin(); it != rows.second.constEnd(); ++it)
			s.select(model()->index(*it, 0), model()->index(*it, model()->columnCount() - 1));

		selectionModel()->select(s, QItemSelectionModel::ClearAndSelect);
	}
}
