// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _CONFIGDIALOG_H_
#define _CONFIGDIALOG_H_

#include <kconfigdialog.h>

namespace Ui 
{
class KonstruktorConfigDisplayForm;
class KonstruktorConfigRenderForm;
}

class KonstruktorConfigDialog : public KConfigDialog
{
  public:
	KonstruktorConfigDialog(QWidget *parent = 0L);
	virtual ~KonstruktorConfigDialog();

  private:
	Ui::KonstruktorConfigDisplayForm *uiDisplayForm_;
	Ui::KonstruktorConfigRenderForm *uiRenderForm_;
};

#endif
