// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _ABOUTDIALOG_H_
#define _ABOUTDIALOG_H_

#include <QDialog>

namespace Ui
{
  class AboutDialog;
}

namespace Konstruktor
{

class AboutDialog : public QDialog
{
  Q_OBJECT;

 public:
  AboutDialog(QWidget *parent = 0L);
  ~AboutDialog();

 private:
  void initCredits();
  void initLicense();

 private:
  Ui::AboutDialog *ui_;
};

}

#endif
