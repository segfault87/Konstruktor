// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#include "application.h"
#include "ui_configdisplayform.h"
#include "ui_configrenderform.h"

#include "configdialog.h"

KonstruktorConfigDialog::KonstruktorConfigDialog(QWidget *parent)
	: KConfigDialog(parent, "Settings", KonstruktorApplication::self()->config())
{
	uiDisplayForm_ = new Ui::KonstruktorConfigDisplayForm;
	uiRenderForm_ = new Ui::KonstruktorConfigRenderForm;

	QWidget *wDisplay = new QWidget;
	QWidget *wRender = new QWidget;

	uiDisplayForm_->setupUi(wDisplay);
	uiRenderForm_->setupUi(wRender);

	addPage(wDisplay, KonstruktorApplication::self()->config(), i18n("Display"), "system-run", i18n("Configure..."));
	addPage(wRender, KonstruktorApplication::self()->config(), i18n("Render"), "view-preview", i18n("Configure Renderer"));
}

KonstruktorConfigDialog::~KonstruktorConfigDialog()
{
	delete uiDisplayForm_;
	delete uiRenderForm_;
}
