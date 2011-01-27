// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _PARTSICONWIDGET_H_
#define _PARTSICONWIDGET_H_

#include <QListWidget>

class QMimeData;

namespace Konstruktor
{

class PartsIconWidget : public QListWidget
{
  public:
	PartsIconWidget(QWidget *parent = 0L);
	~PartsIconWidget();

  private:
	QMimeData* mimeData(const QList<QListWidgetItem *> items) const;
	QStringList mimeTypes() const;
};

}

#endif
