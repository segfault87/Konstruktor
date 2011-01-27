// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _NEWSUBMODELDIALOG_H_
#define _NEWSUBMODELDIALOG_H_

#include <QDialog>

namespace Ui { class NewSubmodelDialog; }

namespace Konstruktor
{

class NewSubmodelDialog : public QDialog
{
	Q_OBJECT;

  public:
	NewSubmodelDialog(QWidget *parent = 0L);
	virtual ~NewSubmodelDialog();

	QString textName() const;
	QString textDesc() const;

  private slots:
	void textChanged(const QString &text);

  private:
	Ui::NewSubmodelDialog *ui_;
};

}

#endif

