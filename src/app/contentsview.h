// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _CONTENTSVIEW_H_
#define _CONTENTSVIEW_H_

#include <list>
#include <set>

#include <QSet>
#include <QTreeView>

#include "commandbase.h"
#include "renderwidget.h"

namespace ldraw
{
    class model;
};

class KonstruktorContentsView : public QTreeView
{
	Q_OBJECT;
	
  public:
	KonstruktorContentsView(QWidget *parent = 0L);
	virtual ~KonstruktorContentsView();

	const QSet<int>& selected() const { return selectedIndices_; }
	int uniqueSelection() const;

  signals:
	void selectionChanged(const QSet<int> &selectionSet);

  public slots:
	void hide(const QModelIndex &index);
	void unhide(const QModelIndex &index);
	void hideSelected();
	void unhideAll();
	void modelChanged(ldraw::model *model);
	void updateSelection(const std::list<int> &selection, KonstruktorRenderWidget::SelectionMethod method);
	void rowsChanged(const QPair<KonstruktorCommandBase::AffectedRow, QSet<int> > &rows);

  private slots:
	void selectionChanged(const QItemSelection &selected, const QItemSelection &unselected);

  private:
	QSet<int> selectedIndices_;
	std::set<int> *hiddenIndices_;
	ldraw::model *model_;
};

#endif
