// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include "application.h"
#include "ui_configdisplayform.h"
#include "ui_configrenderform.h"

#include "configdialog.h"

namespace Konstruktor
{

ConfigDialog::ConfigDialog()
    : QDialog()
{
  uiDisplayForm_ = new Ui::ConfigDisplayForm;
  uiRenderForm_ = new Ui::ConfigRenderForm;
  
  QWidget *wDisplay = new QWidget;
  QWidget *wRender = new QWidget;
  
  uiDisplayForm_->setupUi(wDisplay);
  uiRenderForm_->setupUi(wRender);
  
  
}

ConfigDialog::~ConfigDialog()
{
  delete uiDisplayForm_;
  delete uiRenderForm_;
}

}
