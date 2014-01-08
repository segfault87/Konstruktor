// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _DBUPDATERDIALOG_H_
#define _DBUPDATERDIALOG_H_

#include <QProcess>
#include <QProgressDialog>

class QProgressDialog;

namespace Konstruktor
{

class DBUpdaterDialog : public QProgressDialog
{
  Q_OBJECT;

 public:
  DBUpdaterDialog(QWidget *parent = 0L);
  ~DBUpdaterDialog();

 public slots:
  void start();
  void start(const QString &path);

 signals:
  void updateFinished();

 private slots:
  void dbUpdateStatus();
  void dbUpdateFinished(int, QProcess::ExitStatus);
  void dbUpdateError(QProcess::ProcessError);

 private:
  QWidget *parent_;
  QProcess *process_;
  int lines_;
};

}

#endif
