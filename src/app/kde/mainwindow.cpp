// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <kaction.h>
#include <kactioncollection.h>

#include "mainwindow.h"

namespace Konstruktor
{

namespace Platform
{

MainWindow::MainWindow(QWidget *parent)
	: KXmlGuiWindow(parent)
{

}

MainWindow::~MainWindow()
{
	
}

void MainWindow::setupPlatformGui()
{
	setupGUI();
}

QAction* MainWindow::createAction(const char *actionName, const QString &name, QObject *receiver, const char *slot, const QString &shortcut, const QString &icon)
{
	KAction *action = actionCollection()->addAction(actionName);
	action->setText(name);
	if (!icon.isEmpty())
		action->setIcon(KIcon(icon));
	if (!shortcut.isEmpty())
		action->setShortcut(shortcut);
	if (receiver && slot)
		connect(action, SIGNAL(triggered()), receiver, slot);

	return action;
}

}

}
