// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _CONFIGDIALOG_H_
#define _CONFIGDIALOG_H_

#include <QDialog>
#include <QIcon>

namespace Ui 
{
  class ConfigDialog;
  class ConfigPageDisplay;
  class ConfigPageLDraw;
  class ConfigPageRender;
}

class QListWidget;
class QListWidgetItem;
class QStackedWidget;

namespace Konstruktor
{

class Config;

class ConfigDialog : public QDialog
{
  Q_OBJECT;

 public:
  ConfigDialog(QWidget *parent);
  virtual ~ConfigDialog();

  void writeToConfig();

 signals:
  void loaded();

 private:
  void syncAll();
  void addPage(const QString &name, const QIcon &icon, QWidget *widget, const char *slot);

 private slots:
  void accept();

  void changePage(QListWidgetItem *current, QListWidgetItem *previous);
  void evaluateLDrawPath(const QString &path);

  void initializeLDraw();
  void initializeDisplay();
  void initializeRender();
  
 private:
  Config *config_;

  Ui::ConfigDialog *uiConfigDialog_;

  Ui::ConfigPageDisplay *uiDisplayForm_;

  Ui::ConfigPageLDraw *uiLDrawForm_;
  bool ldrawPathChanged;
  bool ldrawPathCorrect;

  Ui::ConfigPageRender *uiRenderForm_;

};

}

#endif
