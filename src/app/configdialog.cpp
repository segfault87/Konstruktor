// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QDir>
#include <QMessageBox>

#include "application.h"
#include "dbupdaterdialog.h"
#include "ui_configdialog.h"
#include "ui_configdisplayform.h"
#include "ui_configldrawform.h"
#include "ui_configrenderform.h"

#include "configdialog.h"

namespace Konstruktor
{

ConfigDialog::ConfigDialog(QWidget *parent)
    : QDialog(parent)
{
  config_ = Application::self()->config();

  uiConfigDialog_ = new Ui::ConfigDialog;
  uiConfigDialog_->setupUi(this);
  
  uiLDrawForm_ = new Ui::ConfigPageLDraw;
  uiDisplayForm_ = new Ui::ConfigPageDisplay;
  uiRenderForm_ = new Ui::ConfigPageRender;
  
  QWidget *wLDraw = new QWidget;
  QWidget *wDisplay = new QWidget;
  QWidget *wRender = new QWidget;
  
  uiLDrawForm_->setupUi(wLDraw);
  uiDisplayForm_->setupUi(wDisplay);
  uiRenderForm_->setupUi(wRender);

  addPage(tr("LDraw"), QIcon(), wLDraw, SLOT(initializeLDraw()));
  addPage(tr("Display"), QIcon(), wDisplay, SLOT(initializeDisplay()));
  addPage(tr("Renderer"), QIcon(), wRender, SLOT(initializeRender()));

  uiConfigDialog_->listWidget->setCurrentRow(0);

  connect(uiConfigDialog_->listWidget,
          SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
          this,
          SLOT(changePage(QListWidgetItem *, QListWidgetItem *)));        

  emit loaded();
}

ConfigDialog::~ConfigDialog()
{
  delete uiConfigDialog_;
  delete uiDisplayForm_;
  delete uiRenderForm_;
}

void ConfigDialog::syncAll()
{
  
}

void ConfigDialog::addPage(const QString &name, const QIcon &icon, QWidget *widget, const char *slot)
{
  uiConfigDialog_->pagesWidget->addWidget(widget);

  QListWidgetItem *item = new QListWidgetItem(uiConfigDialog_->listWidget);

  item->setIcon(icon);
  item->setText(name);
  item->setTextAlignment(Qt::AlignHCenter);
  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

  connect(this, SIGNAL(loaded()), this, slot);
}

void ConfigDialog::evaluateLDrawPath(const QString &path)
{
  bool p = false;
  bool parts = false;

  QDir dir(path);
  if (!dir.exists())
    goto fail;

  {
    QStringList entries = dir.entryList(QDir::Dirs);
    foreach (const QString &s, entries) {
      if (s.toLower() == "p")
        p = true;
      else if (s.toLower() == "parts")
        parts = true;
    }
  }

  if (p && parts) {
    uiLDrawForm_->statusLDrawPath->setText(tr("<qt>Status: <font color=green>Confirmed.</font></qt>"));
    ldrawPathCorrect = true;

    return;
  }

fail:
  uiLDrawForm_->statusLDrawPath->setText(tr("<qt>Status: <font color=red>Incorrect path.</font></qt>"));
  ldrawPathCorrect = false;
}

void ConfigDialog::accept()
{
  if (ldrawPathChanged) {
    if (!ldrawPathCorrect) {
      QMessageBox::critical(this, tr("Error"), tr("Please specify proper LDraw path."));

      return;
    }

    if (QMessageBox::question(this,
                              tr("Question"),
                              tr("By changing LDraw path, You must rebuild your entire part database. Proceed?"),
                              QMessageBox::Yes | QMessageBox::Cancel,
                              QMessageBox::Cancel) == QMessageBox::Yes) {
      DBUpdaterDialog updater;
      updater.start(uiLDrawForm_->editLDrawPath->text().toLocal8Bit().constData(), true);
    } else {
      return;
    }
  }

  syncAll();

  QDialog::accept();
}

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
  if (!current)
    current = previous;

  uiConfigDialog_->pagesWidget->setCurrentIndex(uiConfigDialog_->listWidget->row(current));
}

void ConfigDialog::initializeLDraw()
{
  ldrawPathChanged = false;
  ldrawPathCorrect = false;

  connect(uiLDrawForm_->editLDrawPath,
          SIGNAL(textChanged(const QString &)),
          this,
          SLOT(evaluateLDrawPath(const QString &)));

  uiLDrawForm_->editLDrawPath->setText(config_->path());
}

void ConfigDialog::initializeDisplay()
{

}

void ConfigDialog::initializeRender()
{

}

}
