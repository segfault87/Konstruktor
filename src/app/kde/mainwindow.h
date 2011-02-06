// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _MAINWINDOW_KDE_H_
#define _MAINWINDOW_KDE_H_

#include <kshortcut.h>
#include <kxmlguiwindow.h>

#include "mainwindow.h"

class KActionCollection;
class KRecentFilesAction;
class KTabBar;
class KUrl;

namespace Konstruktor 
{

namespace Platform
{


class MainWindow : public KXmlGuiWindow
{
  public:
	MainWindow(QWidget *parent = 0L);
	virtual ~MainWindow();

  protected:
	void setupPlatformGui();
	
	QAction* createAction(const char *actionName, const QString &name, QObject *receiver, const char *slot, const QString &shortcut = QString(), const QString &icon = QString());
};

}

}

#endif
