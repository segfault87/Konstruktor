// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _SUBMODELWIDGET_H_
#define _SUBMODELWIDGET_H_

#include <QTreeView>

class QContextMenuEvent;

class KonstruktorSubmodelWidget : public QTreeView
{
	Q_OBJECT;

  public:
	KonstruktorSubmodelWidget(QWidget *parent = 0L);
	virtual ~KonstruktorSubmodelWidget();

  private:
	void contextMenuEvent(QContextMenuEvent *event);
};

#endif
