// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include "application.h"
#include "ui_configdisplayform.h"
#include "ui_configrenderform.h"

#include "configdialog.h"

namespace Konstruktor
{

ConfigDialog::ConfigDialog(QWidget *parent)
	: KConfigDialog(parent, "Settings", Application::self()->config())
{
	uiDisplayForm_ = new Ui::ConfigDisplayForm;
	uiRenderForm_ = new Ui::ConfigRenderForm;

	QWidget *wDisplay = new QWidget;
	QWidget *wRender = new QWidget;

	uiDisplayForm_->setupUi(wDisplay);
	uiRenderForm_->setupUi(wRender);

	addPage(wDisplay, Application::self()->config(), i18n("Display"), "system-run", i18n("Configure..."));
	addPage(wRender, Application::self()->config(), i18n("Render"), "view-preview", i18n("Configure Renderer"));
}

ConfigDialog::~ConfigDialog()
{
	delete uiDisplayForm_;
	delete uiRenderForm_;
}

}
