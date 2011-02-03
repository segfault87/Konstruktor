// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _SUBMODELWIDGET_H_
#define _SUBMODELWIDGET_H_

#include <QTreeView>

class QContextMenuEvent;

namespace ldraw
{
	class model;
}

namespace Konstruktor
{

class SubmodelWidget : public QTreeView
{
	Q_OBJECT;

  public:
	SubmodelWidget(QWidget *parent = 0L);
	virtual ~SubmodelWidget();

  public slots:
	void modelChanged(ldraw::model *m);

  private:
	void contextMenuEvent(QContextMenuEvent *event);

	QModelIndex previous_;
};

}

#endif
