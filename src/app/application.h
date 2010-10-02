// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

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

class KProgressDialog;
class KProcess;

class KonstruktorColorManager;
class KonstruktorDBManager;
class KonstruktorMainWindow;

// Main application entrypoint
class KonstruktorApplication : public QObject
{
	Q_OBJECT;
	
  public:
	KonstruktorApplication(QObject *parent = 0L);
	~KonstruktorApplication();
	
	bool initialize();
	void startDBUpdater();
	void startup();
	
	static KonstruktorApplication* self() { return instance_; }
	
	void testPovRay(bool overrideconfig = false);
	
	QString saveLocation(const QString &directory);
	ldraw::part_library* library() { return library_; }
	KonstruktorConfig* config() { return config_; }
	KonstruktorDBManager* database() { return db_; }
	KonstruktorColorManager* colorManager() { return colorManager_; }
	bool hasPovRay() const { return hasPovRay_; }

  private slots:
	void dbUpdateStatus();
	void dbUpdateFinished(int, QProcess::ExitStatus);
		
  private:
	static KonstruktorApplication *instance_;
	
	KonstruktorMainWindow *window_;
	KProgressDialog *dbDialog_;
	KProcess *dbUpdater_;
	KonstruktorConfig *config_;
	ldraw::part_library *library_;
	KonstruktorDBManager *db_;
	KonstruktorColorManager *colorManager_;
	QMutex globalDirsMutex_;
	bool hasPovRay_;
};

#endif
