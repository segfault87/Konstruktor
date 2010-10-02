// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#include <cstdlib>

#include <QGLFormat>
#include <QPixmapCache>

#include <kapplication.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprogressdialog.h>
#include <kprocess.h>
#include <kstandarddirs.h>

#include <libldr/color.h>
#include <libldr/model.h>
#include <libldr/part_library.h>

#include "colormanager.h"
#include "dbmanager.h"
#include "mainwindow.h"

#include "application.h"

KonstruktorApplication* KonstruktorApplication::instance_ = 0L;

KonstruktorApplication::KonstruktorApplication(QObject *parent)
	: QObject(parent)
{
	instance_ = this;
	
	ldraw::color::init();

	config_ = new KonstruktorConfig;
	db_ = new KonstruktorDBManager(this);
	colorManager_ = new KonstruktorColorManager;

	dbDialog_ = 0L;

	if (!initialize())
		kapp->exit();
}

KonstruktorApplication::~KonstruktorApplication()
{
	instance_ = 0L;
	
	delete colorManager_;
	delete library_;

	config_->writeConfig();
	delete config_;
}

bool KonstruktorApplication::initialize()
{
	int attempt = 0;
	bool retry;
	std::string path = config_->path().toLocal8Bit().data();
	
	do {
		retry = false;
		
		try {
			if (path.empty())
				library_ = new ldraw::part_library;
			else
				library_ = new ldraw::part_library(path);
		} catch (const ldraw::exception &) {
			KMessageBox::error(0L, i18n("<qt>Unable to find LDraw part library. If you have installed LDraw, please specify your installation path.  If you have not installed it, it can be obtained from <a href=\"http://www.ldraw.org\">http://www.ldraw.org</a>.</qt>"));
			
			QString newpath = KFileDialog::getExistingDirectory(KUrl(), 0L, i18n("Choose LDraw installation directory"));
			if (newpath.isEmpty()) {
				// Last attempt
				if (!config_->path().isEmpty()) {
					try {
						library_ = new ldraw::part_library;
					} catch(...) {
						return false;
					}
					
					config_->setPath("");
					config_->writeConfig();
				} else {
					return false;
				}
			} else {
				retry = true;
				path = newpath.toLocal8Bit().data();
				++attempt;
			}
		}
	} while (retry);
	
	if (attempt) {
		config_->setPath(path.data());
		config_->writeConfig();
	}
	
	db_->initialize(saveLocation("")+"parts.db");

	testPovRay(true);

	startDBUpdater();

	return true;
}

void KonstruktorApplication::startDBUpdater()
{
	dbUpdater_ = new KProcess(this);
	dbUpdater_->setOutputChannelMode(KProcess::SeparateChannels);
	*dbUpdater_ << "konstruktor_db_updater" << library_->ldrawpath().c_str();

	connect(dbUpdater_, SIGNAL(readyReadStandardOutput()), this, SLOT(dbUpdateStatus()));
	connect(dbUpdater_, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(dbUpdateFinished(int, QProcess::ExitStatus)));

	dbUpdater_->start();
}

void KonstruktorApplication::startup()
{
	window_ = new KonstruktorMainWindow();
	
	window_->show();
}

QString KonstruktorApplication::saveLocation(const QString &directory)
{
	globalDirsMutex_.lock();
	QString result = KGlobal::dirs()->saveLocation("data", QString("konstruktor/") + directory, true);
	globalDirsMutex_.unlock();
	
	return result;
}

void KonstruktorApplication::testPovRay(bool overrideconfig)
{
	if (!config_->povRayExecutablePath().isEmpty()) {
		QStringList args;

		args << config_->povRayExecutablePath() << "--version";
		if (KProcess::execute(args) != 0) {
			KMessageBox::error(0L, i18n("Could not execute POV-Ray. Raytracing feature is temporarily disabled. Please make sure that POV-Ray is properly installed."));

			if (overrideconfig) {
				config_->setPovRayExecutablePath("");
				config_->writeConfig();
			}
			
			hasPovRay_ = false;
		} else {
			hasPovRay_ = true;
		}
	} else {
		if (KProcess::execute("povray") >= 0) {
			config_->setPovRayExecutablePath("povray");
			config_->writeConfig();
			hasPovRay_ = true;
		} else {
			hasPovRay_ = false;
		}
	}			
}

void KonstruktorApplication::dbUpdateStatus()
{
	if (!dbDialog_) {
		dbDialog_ = new KProgressDialog(0L, i18n("Scanning"), i18n("<qt><p align=center>Konstruktor is now creating database from LDraw part library in your system. Please wait...<br/>%1</p></qt>", QString()));
		dbDialog_->setAutoClose(true);
		dbDialog_->showCancelButton(false);
		dbDialog_->show();
	}
	
	dbUpdater_->setReadChannel(KProcess::StandardOutput);

	QStringList message = QString(dbUpdater_->readAll()).trimmed().split('\n');
	QString lastLine = message[message.size() - 1].trimmed();

	int cur = lastLine.section(' ', 0, 0).toInt();
	int max = lastLine.section(' ', 1, 1).toInt();

	dbDialog_->progressBar()->setMaximum(max);
	dbDialog_->progressBar()->setValue(cur);
	dbDialog_->setLabelText(i18n("<qt><p align=center>Konstruktor is now creating database from LDraw part library in your system. Please wait...<br/>%1</p></qt>", lastLine.section(' ', 2)));
}

void KonstruktorApplication::dbUpdateFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (dbDialog_) {
		delete dbDialog_;
		dbDialog_ = 0L;
	}
	
	delete dbUpdater_;
	dbUpdater_ = 0L;

	if (exitCode || exitStatus == QProcess::CrashExit) {
		KMessageBox::sorry(0L, i18n("Could not scan LDraw part library."));
		kapp->exit();
	} else {
		startup();
	}
}

