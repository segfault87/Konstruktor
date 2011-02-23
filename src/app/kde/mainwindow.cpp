// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <kaction.h>
#include <kactioncollection.h>
#include <kfiledialog.h>
#include <klocalizedstring.h>

#include "document.h"

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

QString MainWindow::platformSaveFileDialog(Document *document)
{
	KFileDialog dialog(KUrl(), QString(), this);
	dialog.setCaption(i18n("Save document"));
	dialog.setOperationMode(KFileDialog::Saving);
	QStringList mimes;

	bool isSubmodelEmpty = !document->contents()->submodel_list().size();
	if (isSubmodelEmpty)
		mimes << "application/x-ldraw";
	mimes << "application/x-multi-part-ldraw";
	
	dialog.setMimeFilter(mimes, isSubmodelEmpty ? "application/x-ldraw" : "application/x-multi-part-ldraw");

	dialog.exec();

	KUrl url = dialog.selectedUrl();
	bool isMultipart = dialog.currentMimeFilter() == "application/x-multi-part-ldraw";

	if (url.isEmpty())
		return QString();

	QString localFileName = url.toLocalFile();
	
	if (!url.fileName().contains(".")) {
		if (isMultipart)
			localFileName += ".mpd";
		else
			localFileName += ".ldr";
	}

	return localFileName;
}

bool MainWindow::platformSaveFile(const QString &url, bool isMultipart)
{
	return false;
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
