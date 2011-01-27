// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <cstdlib>

#include <QPushButton>

#include "ui_newmodeldialog.h"

#include "newmodeldialog.h"

namespace Konstruktor
{

NewModelDialog::NewModelDialog(QWidget *parent) 
	: QDialog(parent)
{
	ui_ = new Ui::NewModelDialog;

	ui_->setupUi(this);
	ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

	const char *username = std::getenv("USER");
	if (username)
		ui_->editAuthor->setText(username);

	connect(ui_->editDesc, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
	connect(ui_->editAuthor, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
}

NewModelDialog::~NewModelDialog()
{
	delete ui_;
}

QString NewModelDialog::textDesc() const
{
	return ui_->editDesc->text();
}

QString NewModelDialog::textAuthor() const
{
	return ui_->editAuthor->text();
}

void NewModelDialog::textChanged(const QString &)
{
	if (ui_->editDesc->text().isEmpty() ||
		ui_->editAuthor->text().isEmpty())
		ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	else
		ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

}
