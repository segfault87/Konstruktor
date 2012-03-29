// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QFile>
#include <QTextBrowser>
#include <QTextEdit>
#include <QTextStream>

#include "version.h"
#include "ui_aboutdialog.h"

#include "aboutdialog.h"

namespace Konstruktor
{

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
  ui_ = new Ui::AboutDialog();

  ui_->setupUi(this);
  
  ui_->labelVersion->setText(VERSION);
  ui_->labelCopyright->setText(COPYRIGHT_STRING);
  ui_->labelWebsite->setText(WEBSITE);

  initCredits();
  initLicense();
}

AboutDialog::~AboutDialog()
{
  delete ui_;
}

void AboutDialog::initCredits()
{
  QFile file(":/files/credits.html");
  file.open(QFile::ReadOnly);

  QTextStream in(&file);
  ui_->viewCredits->setHtml(in.readAll());
  
  file.close();
}

void AboutDialog::initLicense()
{
  QFile file(":/files/license.txt");
  file.open(QFile::ReadOnly);

  QTextStream in(&file);
  ui_->viewLicense->setText(in.readAll());
  
  file.close();
}

}
