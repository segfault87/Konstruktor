// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QContextMenuEvent>
#include <QMenu>

#include <kaction.h>
#include <klocale.h>

#include "submodelwidget.h"

namespace Konstruktor
{

SubmodelWidget::SubmodelWidget(QWidget *parent)
{
	setRootIsDecorated(false);
	setHeaderHidden(true);
	setVerticalScrollMode(QTreeView::ScrollPerPixel);
	setDragEnabled(true);
}

SubmodelWidget::~SubmodelWidget()
{

}

void SubmodelWidget::contextMenuEvent(QContextMenuEvent *event)
{
	if (selectedIndexes().count() != 1) {
		event->ignore();
		return;
	}
	
	QMenu contextMenu(this);
	contextMenu.addAction(new KAction(KIcon("edit-copy"), i18n("&Duplicate"), &contextMenu));

	QAction *action = contextMenu.exec(event->globalPos());
	if (action) {


	}

	event->accept();
}

}
