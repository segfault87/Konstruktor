// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _DBUPDATERDIALOG_H_
#define _DBUPDATERDIALOG_H_

#include <QProcess>
#include <QProgressDialog>

class QProgressDialog;

namespace Konstruktor
{

class DBUpdater;

class DBUpdaterDialog : public QProgressDialog
{
  Q_OBJECT;

 public:
  DBUpdaterDialog(QWidget *parent = 0L);
  ~DBUpdaterDialog();

 public slots:
  void start(bool rescan = false);
  void start(const std::string &path, bool rescan = false);

 signals:
  void updateFinished();

 private slots:
  void progress(int current, int total, const std::string &name, const std::string &desc);
  void finished();

 private:
  QWidget *parent_;
  DBUpdater *worker_;
};

}

#endif
