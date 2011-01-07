// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _NEWMODELDIALOG_H_
#define _NEWMODELDIALOG_H_

#include <QDialog>

namespace Ui { class NewModelDialog; }

class KonstruktorNewModelDialog : public QDialog
{
	Q_OBJECT;

  public:
	KonstruktorNewModelDialog(QWidget *parent = 0L);
	virtual ~KonstruktorNewModelDialog();

	QString textDesc() const;
	QString textAuthor() const;

  private slots:
	void textChanged(const QString &text);

  private:
	Ui::NewModelDialog *ui_;
};

#endif

