// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QMimeData>

#include "partitems.h"

#include "partsiconwidget.h"

KonstruktorPartsIconWidget::KonstruktorPartsIconWidget(QWidget *parent)
	: QListWidget(parent)
{
	setViewMode(QListWidget::IconMode);
	setMovement(QListWidget::Static);
	setResizeMode(QListWidget::Adjust);
	setDragEnabled(true);
}

KonstruktorPartsIconWidget::~KonstruktorPartsIconWidget()
{

}

QStringList KonstruktorPartsIconWidget::mimeTypes() const
{	
	QStringList types;
	types << "application/konstruktor-refobject";
	return types;
}

QMimeData* KonstruktorPartsIconWidget::mimeData(const QList<QListWidgetItem *> items) const
{
	if (items.size() != 1)
		return 0L;

	KonstruktorPartItem item = items[0]->data(Qt::UserRole).value<KonstruktorPartItem>();
	
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData = item.serialize();

	mimeData->setData("application/konstruktor-refobject", encodedData);
	return mimeData;
}
