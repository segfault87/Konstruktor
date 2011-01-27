// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _CONFIGDIALOG_H_
#define _CONFIGDIALOG_H_

#include <kconfigdialog.h>

namespace Ui 
{
class ConfigDisplayForm;
class ConfigRenderForm;
}

namespace Konstruktor
{

class ConfigDialog : public KConfigDialog
{
  public:
	ConfigDialog(QWidget *parent = 0L);
	virtual ~ConfigDialog();

  private:
	Ui::ConfigDisplayForm *uiDisplayForm_;
	Ui::ConfigRenderForm *uiRenderForm_;
};

}

#endif
