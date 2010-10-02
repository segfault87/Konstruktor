// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _PARTSWIDGET_H_
#define _PARTSWIDGET_H_

#include <QList>
#include <QMap>
#include <QWidget>

#include "partitems.h"

namespace Ui { class PartsWidget; }
class QListWidgetItem;
class QSortFilterProxyModel;
class KonstruktorPartsModel;
class QModelIndex;

class KonstruktorPartsWidget : public QWidget
{
	Q_OBJECT;

  public:
	KonstruktorPartsWidget(QWidget *parent = 0L);
	virtual ~KonstruktorPartsWidget();

  public slots:
	void initialize(const QString &search = QString(), bool hideUnofficial = false);
	void hideUnofficial(int checkState);

  private slots:
	void selectionChanged(const QModelIndex &current, const QModelIndex &previous);
	void iconSelected(QListWidgetItem *item);

  private:
	Ui::PartsWidget *ui_;
	KonstruktorPartsModel *model_;
	QSortFilterProxyModel *sortModel_;

	QString search_;
	bool hideUnofficial_;
	
	QList<KonstruktorPartCategory> categories_;
	QMap<int, KonstruktorPartCategory *> categorymap_;
	QMap<int, QList<KonstruktorPartItem> > list_;

	int lastCat_;
};

#endif

