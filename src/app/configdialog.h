// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _CONFIGDIALOG_H_
#define _CONFIGDIALOG_H_

#include <QDialog>

namespace Ui 
{
class ConfigDisplayForm;
class ConfigRenderForm;
}

class QListWidget;
class QStackedWidget;

namespace Konstruktor
{

class ConfigDialog : public QDialog
{
 public:
  ConfigDialog();
  virtual ~ConfigDialog();
  
 private:
  Ui::ConfigDisplayForm *uiDisplayForm_;
  Ui::ConfigRenderForm *uiRenderForm_;
};

}

#endif
