// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <libldr/elements.h>

#include "contentsmodel.h"

#include "contentsview.h"

namespace Konstruktor
{

ContentsView::ContentsView(QWidget *parent)
    : QTreeView(parent)
{
  setRootIsDecorated(false);
  setSelectionBehavior(QTreeView::SelectRows);
  setSelectionMode(QTreeView::ExtendedSelection);
  
  hiddenIndices_ = 0L;
}

ContentsView::~ContentsView()
{
  
}

int ContentsView::uniqueSelection() const
{
  if (selectedIndices_.size() == 1)
    return *selectedIndices_.begin();
  
  return -1;
}

void ContentsView::hide(const QModelIndex &index)
{
  if (selectedIndices_.contains(index.row()))
    selectedIndices_.remove(index.row());
  
  selectionModel()->clearSelection();
  
  emit selectionChanged(selectedIndices_);
  
  update();
}

void ContentsView::unhide(const QModelIndex &index)
{
  selectionModel()->clearSelection();
  selectionModel()->select(QItemSelection(index, index.model()->index(index.row(), index.model()->columnCount() - 1)), QItemSelectionModel::Select);
  
  emit selectionChanged(selectedIndices_);
  
  update();
}

void ContentsView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
  repaint();
  
  QTreeView::selectionChanged(selected, deselected);
  
  int affected = 0;
  
  const QModelIndexList &list = selected.indexes();
  for (QModelIndexList::ConstIterator it = list.begin(); it != list.end(); ++it) {
    if ((*it).column() == 0 && (*it).isValid() && !selectedIndices_.contains((*it).row())) {
      selectedIndices_.insert((*it).row());
      ++affected;
    }
  }
  
  const QModelIndexList &delist = deselected.indexes();
  for (QModelIndexList::ConstIterator it = delist.begin(); it != delist.end(); ++it) {
    if ((*it).column() == 0 && (*it).isValid() && selectedIndices_.contains((*it).row())) {
      if (selectedIndices_.contains((*it).row()))
        selectedIndices_.remove((*it).row());
      
      ++affected;
    }
  }
  
  if (affected)
    emit selectionChanged(selectedIndices_);
}

void ContentsView::hideSelected()
{
  dynamic_cast<ContentsModel *>(model())->hideSelected(selectedIndices_);
  
  selectedIndices_.clear();
  
  emit selectionChanged(selectedIndices_);
  
  clearSelection();
}

void ContentsView::unhideAll()
{
  const std::set<int> hset = *hiddenIndices_;
  
  dynamic_cast<ContentsModel *>(model())->unhideAll();
  
  selectedIndices_.clear();
  for (std::set<int>::const_iterator it = hset.begin(); it != hset.end(); ++it) {
    QModelIndex start = model()->index(*it, 0);
    QModelIndex end = model()->index(*it, model()->columnCount() - 1);
    
    selectionModel()->select(QItemSelection(start, end), QItemSelectionModel::Select);
    selectedIndices_.insert(*it);
  }
  
  emit selectionChanged(selectedIndices_);
}

void ContentsView::modelChanged(ldraw::model *m)
{
  model_ = m;
  
  selectedIndices_.clear();
  
  if (!model_)
    return;
  
  hiddenIndices_ = &VisibilityExtension::query(model_)->set();
}

void ContentsView::updateSelection(const std::list<int> &selection, RenderWidget::SelectionMethod method)
{
  const int maxcol = model()->columnCount() - 1;
  
  QItemSelection s;
  
  if (method == RenderWidget::Intersection) {
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
  
  if (method == RenderWidget::Addition)
    flag = QItemSelectionModel::Select;
  else if (method == RenderWidget::Subtraction)
    flag = QItemSelectionModel::Deselect;
  
  selectionModel()->select(s, flag);
}

void ContentsView::rowsChanged(const QPair<CommandBase::AffectedRow, QSet<int> > &rows)
{
  // TODO preserve visibility
  
  if (!rows.second.size())
    return;
  
  selectedIndices_.clear();
  hiddenIndices_->clear();
  
  if (rows.first == CommandBase::Removed)
    clearSelection();
  else {
    QItemSelection s;
    for (QSet<int>::ConstIterator it = rows.second.constBegin(); it != rows.second.constEnd(); ++it)
      s.select(model()->index(*it, 0), model()->index(*it, model()->columnCount() - 1));
    
    selectionModel()->select(s, QItemSelectionModel::ClearAndSelect);
  }
}

}
