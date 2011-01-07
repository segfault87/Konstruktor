// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QPushButton>

#include "ui_newsubmodeldialog.h"

#include "newsubmodeldialog.h"

KonstruktorNewSubmodelDialog::KonstruktorNewSubmodelDialog(QWidget *parent) 
	: QDialog(parent)
{
	ui_ = new Ui::NewSubmodelDialog;

	ui_->setupUi(this);
	ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

	connect(ui_->editName, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
	connect(ui_->editDesc, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
}

KonstruktorNewSubmodelDialog::~KonstruktorNewSubmodelDialog()
{
	delete ui_;
}

QString KonstruktorNewSubmodelDialog::textName() const
{
	return ui_->editName->text() + ".ldr";
}

QString KonstruktorNewSubmodelDialog::textDesc() const
{
	return ui_->editDesc->text();
}

void KonstruktorNewSubmodelDialog::textChanged(const QString &)
{
	if (ui_->editDesc->text().isEmpty() ||
		ui_->editName->text().isEmpty())
		ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	else
		ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}


