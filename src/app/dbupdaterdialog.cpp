// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QApplication>
#include <QDir>
#include <QMessageBox>

#include <libldr/part_library.h>

#include "application.h"
#include "dbupdater.h"

#include "dbupdaterdialog.h"

namespace Konstruktor
{

DBUpdaterDialog::DBUpdaterDialog(QWidget *parent)
    : QProgressDialog(parent)
{
  worker_ = 0L;
}

DBUpdaterDialog::~DBUpdaterDialog()
{
  if (worker_) {
    worker_->quit();
    worker_->wait();
    delete worker_;
  }
}

void DBUpdaterDialog::start(bool rescan)
{
  start(Application::self()->library()->ldrawpath(), rescan);
}

void DBUpdaterDialog::start(const std::string &path, bool rescan)
{
  if (worker_)
    return;

  worker_ = new DBUpdater(path, rescan);
  connect(worker_, SIGNAL(progress(int, int, const std::string &, const std::string &)),
          this, SLOT(progress(int, int, const std::string &, const std::string &)));
  connect(worker_, SIGNAL(finished()),
          this, SLOT(finished()));
  connect(worker_, SIGNAL(scanFinished()),
          this, SLOT(finished()));

#ifdef Q_OS_WIN32
  /* we have threading issue in Win32 so do not run as a separate thread */
  worker_->runSingleThreaded();
#else
  worker_->runSingleThreaded();
#endif
}

void DBUpdaterDialog::progress(int current, int total, const std::string &name, const std::string &desc)
{
  if (current % 10 != 0)
    return;

  setMaximum(total);
  setValue(current);
  setLabelText(tr("<qt><p align=center>Building indexes from the LDraw part library. Please wait...<br/>%1 (%2)</p></qt>").arg(desc.c_str()).arg(name.c_str()));
}

void DBUpdaterDialog::finished()
{
  accept();
}

}
