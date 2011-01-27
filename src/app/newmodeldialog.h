// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _NEWMODELDIALOG_H_
#define _NEWMODELDIALOG_H_

#include <QDialog>

namespace Ui { class NewModelDialog; }

namespace Konstruktor
{

class NewModelDialog : public QDialog
{
	Q_OBJECT;

  public:
	NewModelDialog(QWidget *parent = 0L);
	virtual ~NewModelDialog();

	QString textDesc() const;
	QString textAuthor() const;

  private slots:
	void textChanged(const QString &text);

  private:
	Ui::NewModelDialog *ui_;
};

}

#endif

