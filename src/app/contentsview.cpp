// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#include <libldr/elements.h>

#include "contentsmodel.h"

#include "contentsview.h"

KonstruktorContentsView::KonstruktorContentsView(QWidget *parent)
	: QTreeView(parent)
{
	setRootIsDecorated(false);
	setSelectionBehavior(QTreeView::SelectRows);
	setSelectionMode(QTreeView::ExtendedSelection);
}

KonstruktorContentsView::~KonstruktorContentsView()
{
	
}

int KonstruktorContentsView::uniqueSelection() const
{
	if (selectedIndexes_.size() == 1)
		return *selectedIndexes_.begin();

	return -1;
}

void KonstruktorContentsView::hide(const QModelIndex &index)
{
	if (selectedIndexes_.contains(index.row())) {
		hiddenIndexes_.insert(index.row());
		selectedIndexes_.remove(index.row());

		emit selectionChanged(selectedIndexes_);
	}
}

void KonstruktorContentsView::unhide(const QModelIndex &index)
{
	if (hiddenIndexes_.contains(index.row())) {
		hiddenIndexes_.remove(index.row());

		if (selectionModel()->isSelected(index))
			selectedIndexes_.insert(index.row());

		emit selectionChanged(selectedIndexes_);
	}
}

void KonstruktorContentsView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	repaint();
	
	QTreeView::selectionChanged(selected, deselected);
	
	int affected = 0;

	const QModelIndexList &list = selected.indexes();
	for (QModelIndexList::ConstIterator it = list.begin(); it != list.end(); ++it) {
		if ((*it).column() == 0 && (*it).isValid() && !selectedIndexes_.contains((*it).row())) {
			if (static_cast<ldraw::element_base *>((*it).internalPointer())->line_type() == 0)
				continue;
			
			selectedIndexes_.insert((*it).row());
			++affected;
		}
	}

	const QModelIndexList &delist = deselected.indexes();
	for (QModelIndexList::ConstIterator it = delist.begin(); it != delist.end(); ++it) {
		if ((*it).column() == 0 && (*it).isValid() && selectedIndexes_.contains((*it).row())) {
			if (static_cast<ldraw::element_base *>((*it).internalPointer())->line_type() == 0)
					continue;
			
			if (selectedIndexes_.contains((*it).row()))
				selectedIndexes_.remove((*it).row());
			else if (hiddenIndexes_.contains((*it).row()))
				hiddenIndexes_.remove((*it).row());

			++affected;
		}
	}
	
	if (affected)
	  emit selectionChanged(selectedIndexes_);
}

void KonstruktorContentsView::hideSelected()
{
	dynamic_cast<KonstruktorContentsModel *>(model())->hideSelected(selectedIndexes_);

	for (QSet<int>::ConstIterator it = selectedIndexes_.constBegin(); it != selectedIndexes_.constEnd(); ++it)
		hiddenIndexes_.insert(*it);
	selectedIndexes_.clear();

	emit selectionChanged(selectedIndexes_);
}

void KonstruktorContentsView::unhideAll()
{
	dynamic_cast<KonstruktorContentsModel *>(model())->unhideAll();

	selectedIndexes_.clear();
	hiddenIndexes_.clear();
	QModelIndexList selection = selectionModel()->selection().indexes();
	for (QModelIndexList::Iterator it = selection.begin(); it != selection.end(); ++it)
		selectedIndexes_.insert((*it).row());

	emit selectionChanged(selectedIndexes_);
}

void KonstruktorContentsView::modelChanged(ldraw::model *)
{
	selectedIndexes_.clear();
	hiddenIndexes_.clear();
}

void KonstruktorContentsView::updateSelection(const std::list<int> &selection)
{
	QItemSelection s;
	for (std::list<int>::const_iterator it = selection.begin(); it != selection.end(); ++it)
		s.select(model()->index(*it, 0), model()->index(*it, model()->columnCount() - 1));

	selectionModel()->select(s, QItemSelectionModel::ClearAndSelect);
}

void KonstruktorContentsView::rowsChanged(const QPair<KonstruktorCommandBase::AffectedRow, QSet<int> > &rows)
{
	// FIXME preserve visibility
	
	if (!rows.second.size())
		return;
		
	selectedIndexes_.clear();
	hiddenIndexes_.clear();

	if (rows.first == KonstruktorCommandBase::Removed)
		clearSelection();
	else {
		QItemSelection s;
		for (QSet<int>::ConstIterator it = rows.second.constBegin(); it != rows.second.constEnd(); ++it)
			s.select(model()->index(*it, 0), model()->index(*it, model()->columnCount() - 1));

		selectionModel()->select(s, QItemSelectionModel::ClearAndSelect);
	}
}
