// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QApplication>
#include <QDir>
#include <QMessageBox>

#include <libldr/part_library.h>

#include "application.h"

#include "dbupdaterdialog.h"

namespace Konstruktor
{

DBUpdaterDialog::DBUpdaterDialog(QWidget *parent)
    : QProgressDialog(parent)
{
  process_ = 0L;
}

DBUpdaterDialog::~DBUpdaterDialog()
{
}

void DBUpdaterDialog::start()
{
  start(Application::self()->library()->ldrawpath().c_str());
}

void DBUpdaterDialog::start(const QString &path)
{
  if (process_)
    return;

  lines_ = 0;

  process_ = new QProcess(this);
  connect(process_, SIGNAL(readyReadStandardOutput()), this, SLOT(dbUpdateStatus()));
  connect(process_, SIGNAL(error(QProcess::ProcessError)), this, SLOT(dbUpdateError(QProcess::ProcessError)));
  connect(process_, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(dbUpdateFinished(int, QProcess::ExitStatus)));

  QStringList args;
  args << path;

  QDir dir(qApp->applicationDirPath());

  process_->start(dir.absoluteFilePath("konstruktor_db_updater"), args);
}

void DBUpdaterDialog::dbUpdateStatus()
{
  if (lines_ % 10 != 0)
    return;

  process_->setReadChannel(QProcess::StandardOutput);

  QStringList message = QString(process_->readAll()).split('\n');
  QString lastLine = message[message.size() - 1].trimmed();

  int cur = lastLine.section(' ', 0, 0).toInt();
  int max = lastLine.section(' ', 1, 1).toInt();

  setMaximum(max);
  setValue(cur);
  setLabelText(tr("<qt><p align=center>Building indexes from the LDraw part library. Please wait...<br/>%1</p></qt>").arg(lastLine.section(' ', 2)));
}

void DBUpdaterDialog::dbUpdateFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  if (exitCode || exitStatus == QProcess::CrashExit) {
    QMessageBox::critical(0L, tr("Error"), tr("Could not scan LDraw part library."));
    reject();
  } else {
    accept();
  }
}

void DBUpdaterDialog::dbUpdateError(QProcess::ProcessError error)
{
  QString errorMsg;
  
  switch (error) {
    case QProcess::FailedToStart:
      errorMsg = tr("Failed to start part database updater. Your installation might be broken.");
      break;
    case QProcess::Crashed:
      errorMsg = tr("Part database updater is stopped unexpectedly.");
      break;
    default:
      errorMsg = tr("Unknown error occurred while scanning parts.");
  }
  
  QMessageBox::critical(0L, tr("Error in Database Updater"), errorMsg);

  reject();
}

}
