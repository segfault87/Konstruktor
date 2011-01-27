// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <QGLFormat>
#include <QMutex>
#include <QObject>
#include <QProcess>
#include <QString>

#include "config.h"

namespace ldraw
{
	class model;
	class part_library;
}

namespace ldraw_renderer
{
	class parameters;
}

class KProgressDialog;
class KProcess;

namespace Konstruktor
{

class ColorManager;
class DBManager;
class MainWindow;

// Main application entrypoint
class Application : public QObject
{
	Q_OBJECT;
	
  public:
	Application(QObject *parent = 0L);
	~Application();
	
	bool initialize();
	void startDBUpdater();
	void startup();
	
	static Application* self() { return instance_; }
	
	void testPovRay(bool overrideconfig = false);
	
	QString saveLocation(const QString &directory);
	ldraw::part_library* library() { return library_; }
	ldraw_renderer::parameters* renderer_params() { return params_; }
	Config* config() { return config_; }
	DBManager* database() { return db_; }
	ColorManager* colorManager() { return colorManager_; }
	QWidget* rootWindow();
	bool hasPovRay() const { return hasPovRay_; }

  public slots:
	void configUpdated();

  private slots:
	void dbUpdateStatus();
	void dbUpdateFinished(int, QProcess::ExitStatus);
	void dbUpdateError(QProcess::ProcessError);
		
  private:
	static Application *instance_;

	MainWindow *window_;
	KProgressDialog *dbDialog_;
	KProcess *dbUpdater_;
	Config *config_;
	ldraw::part_library *library_;
	ldraw_renderer::parameters *params_;
	DBManager *db_;
	ColorManager *colorManager_;
	QMutex globalDirsMutex_;
	bool hasPovRay_;
};

}

#endif
