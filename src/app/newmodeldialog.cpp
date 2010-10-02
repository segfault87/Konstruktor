// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QPushButton>

#include "ui_newmodeldialog.h"

#include "newmodeldialog.h"

KonstruktorNewModelDialog::KonstruktorNewModelDialog(QWidget *parent) 
	: QDialog(parent)
{
	ui_ = new Ui::NewModelDialog;

	ui_->setupUi(this);
	ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

	connect(ui_->editDesc, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
	connect(ui_->editAuthor, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
}

KonstruktorNewModelDialog::~KonstruktorNewModelDialog()
{
	delete ui_;
}

QString KonstruktorNewModelDialog::textDesc() const
{
	return ui_->editDesc->text();
}

QString KonstruktorNewModelDialog::textAuthor() const
{
	return ui_->editAuthor->text();
}

void KonstruktorNewModelDialog::textChanged(const QString &)
{
	if (ui_->editDesc->text().isEmpty() ||
		ui_->editAuthor->text().isEmpty())
		ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	else
		ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}


